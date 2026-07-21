/*
 * ============================================================================
 * [NEV]² - NEV-OS CyberLab v3.0
 * module_wifi_scan.h - Wi-Fi network scanner
 * ============================================================================
 */
#pragma once

#include <Arduino.h>
#include <WiFi.h>
#include <TFT_eSPI.h>
#include "config.h"
#include "types.h"
#include "ui_helpers.h"

extern TFT_eSPI tft;
extern Network _networks[MAX_NETWORKS];
extern int wifiNetworksFound;
extern int wifiAnalyserPage;
extern AppState currentState;

// Build a "Wi-Fi Auth" string from enum (avoids String allocation)
inline const char* encTypeStr(wifi_auth_mode_t enc) {
  switch (enc) {
    case WIFI_AUTH_OPEN:            return "ABERTA";
    case WIFI_AUTH_WEP:             return "WEP";
    case WIFI_AUTH_WPA_PSK:         return "WPA";
    case WIFI_AUTH_WPA2_PSK:        return "WPA2";
    case WIFI_AUTH_WPA_WPA2_PSK:    return "WPA/WPA2";
    case WIFI_AUTH_WPA2_ENTERPRISE: return "WPA2-ENT";
    case WIFI_AUTH_WPA3_PSK:        return "WPA3";
    default:                        return "?";
  }
}

inline void enterWifiScan() {
  currentState = STATE_WIFI_SCAN;
  wifiAnalyserPage = 0;

  tft.fillScreen(COLOR_BG);
  drawHeader("Wi-Fi Scanner", true);
  drawCenteredText("Escaneando redes...", 110, COLOR_WARN, 1);
  drawStatusBar("Aguarde ~5s");

  // Set STA mode
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
  WiFi.scanDelete();
  wifiNetworksFound = WiFi.scanNetworks(false, true);  // async=false, show_hidden=true

  // Populate global _networks array (this is the SOURCE OF TRUTH)
  for (int i = 0; i < wifiNetworksFound && i < MAX_NETWORKS; i++) {
    String s = WiFi.SSID(i);
    strncpy(_networks[i].ssid, s.c_str(), sizeof(_networks[i].ssid) - 1);
    _networks[i].ssid[sizeof(_networks[i].ssid) - 1] = '\0';
    _networks[i].ch = WiFi.channel(i);
    _networks[i].rssi = WiFi.RSSI(i);
    _networks[i].enc = WiFi.encryptionType(i);
    memcpy(_networks[i].bssid, WiFi.BSSID(i), 6);
  }
}

inline void renderWifiScanScreen() {
  tft.fillScreen(COLOR_BG);
  drawHeader("Wi-Fi Scanner", true);

  tft.setTextColor(COLOR_TEXT);
  tft.setTextSize(1);
  tft.setCursor(40, 30);
  if (wifiNetworksFound <= 0) {
    tft.print("Nenhuma rede encontrada");
  } else {
    tft.printf("%d redes | Pag %d", wifiNetworksFound, wifiAnalyserPage + 1);
  }

  // 3 networks per page
  int startIdx = wifiAnalyserPage * 3;
  int ssidY[3] = {50, 110, 170};
  int infoY[3] = {68, 128, 188};

  for (int i = 0; i < 3 && (startIdx + i) < wifiNetworksFound && (startIdx + i) < MAX_NETWORKS; i++) {
    int idx = startIdx + i;
    tft.setTextColor(COLOR_WARN);
    tft.setTextSize(1);
    tft.setCursor(15, ssidY[i]);
    char buf[24];
    strncpy(buf, _networks[idx].ssid, sizeof(buf)-1);
    buf[sizeof(buf)-1] = '\0';
    if (strlen(buf) > 18) { buf[15]='.'; buf[16]='.'; buf[17]='.'; buf[18]='\0'; }
    tft.print(buf);

    tft.setTextColor(COLOR_TEXT);
    tft.setCursor(15, infoY[i]);
    tft.printf("Ch:%2d %ddBm %s",
                _networks[idx].ch, _networks[idx].rssi,
                encTypeStr(_networks[idx].enc));
  }
  drawStatusBar("Curto:Proxima Pag  Longo:Voltar");
}

inline void handleWifiScanShort() {
  wifiAnalyserPage++;
  int maxPages = (wifiNetworksFound + 2) / 3;
  if (maxPages < 1) maxPages = 1;
  if (wifiAnalyserPage >= maxPages) wifiAnalyserPage = 0;
  renderWifiScanScreen();
}
