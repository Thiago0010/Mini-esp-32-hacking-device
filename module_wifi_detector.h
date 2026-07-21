/*
 * ============================================================================
 * [NEV]² - NEV-OS CyberLab v3.0
 * module_wifi_detector.h - Wi-Fi device detector (promiscuous mode)
 *
 * Detects probe requests, beacon frames, and data traffic from devices.
 * Use for education about how WiFi devices leak information.
 * ============================================================================
 */
#pragma once

#include <Arduino.h>
#include <WiFi.h>
#include <TFT_eSPI.h>
#include "config.h"
#include "types.h"
#include "ui_helpers.h"
#include "esp_wifi.h"

extern TFT_eSPI tft;
extern AppState currentState;
extern bool detectorActive;
extern int detectorLogCount;
extern char detectorLog[20][40];

// Promiscuous mode callback - logs interesting frames
inline void IRAM_ATTR wifiPromiscuousCb(void* buf, wifi_promiscuous_pkt_type_t type) {
  if (!detectorActive) return;
  const wifi_promiscuous_pkt_t* pkt = (wifi_promiscuous_pkt_t*)buf;
  const uint8_t* payload = pkt->payload;
  uint16_t len = pkt->rx_ctrl.sig_len;

  if (len < 24) return;

  // Frame Control: type/subtype
  uint8_t fc0 = payload[0];
  uint8_t type = (fc0 & 0x0C) >> 2;
  uint8_t subtype = (fc0 & 0xF0) >> 4;

  // Type 0 = Management, Type 2 = Data
  if (type == 0 && subtype == 4) {
    // Probe Request - log
    if (detectorLogCount < 20) {
      // BSSID/source MAC at offset 10
      snprintf(detectorLog[detectorLogCount], 40, "PROBE %02X:%02X:%02X",
               payload[10], payload[11], payload[12]);
      detectorLogCount++;
    }
  } else if (type == 0 && subtype == 8) {
    // Beacon
    if (detectorLogCount < 20) {
      snprintf(detectorLog[detectorLogCount], 40, "BEACON %02X:%02X:%02X",
               payload[10], payload[11], payload[12]);
      detectorLogCount++;
    }
  }
}

inline void drawDetectorScreen() {
  tft.fillScreen(COLOR_BG);
  drawHeader("Wi-Fi Detector", true);

  drawPanel(20, 30, 280, 35);
  tft.setTextColor(COLOR_TEXT);
  tft.setCursor(30, 42);
  tft.print("Status:");
  tft.setTextColor(detectorActive ? COLOR_DANGER : COLOR_PRIMARY);
  tft.setCursor(80, 42);
  tft.print(detectorActive ? "MONITOR" : "OFF");

  tft.setTextColor(COLOR_TEXT);
  tft.setCursor(140, 42);
  tft.printf("Log:%d", detectorLogCount);

  // Log area
  int startY = 75;
  int maxShow = min(detectorLogCount, 8);
  for (int i = 0; i < maxShow; i++) {
    tft.setTextColor(i == maxShow - 1 ? COLOR_WARN : COLOR_TEXT);
    tft.setCursor(10, startY + i * 16);
    tft.print(detectorLog[detectorLogCount - 1 - i]);
  }

  drawStatusBar("Longo:toggle monitor  BACK:sair");
}

inline void enterWifiDetector() {
  currentState = STATE_WIFI_DETECTOR;
  detectorActive = false;
  detectorLogCount = 0;
  for (int i = 0; i < 20; i++) detectorLog[i][0] = '\0';
  drawDetectorScreen();
}

inline void toggleWifiDetector() {
  detectorActive = !detectorActive;
  if (detectorActive) {
    WiFi.mode(WIFI_STA);
    wifi_promiscuous_filter_t filter = {};
    filter.filter_mask = WIFI_PROMIS_FILTER_MASK_MGMT | WIFI_PROMIS_FILTER_MASK_DATA;
    esp_wifi_set_promiscuous_filter(&filter);
    esp_wifi_set_promiscuous(true);
    esp_wifi_set_promiscuous_rx_cb(&wifiPromiscuousCb);
  } else {
    esp_wifi_set_promiscuous(false);
  }
  drawDetectorScreen();
}
