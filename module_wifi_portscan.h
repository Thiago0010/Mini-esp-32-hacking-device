/*
 * ============================================================================
 * [NEV]² - NEV-OS CyberLab v3.0
 * module_wifi_portscan.h - TCP port scanner
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
extern AppState currentState;
extern PortResult portResults[16];
extern int portScanCurrent;
extern int portScanTotal;
extern bool portScanActive;
extern char portScanTarget[20];

// Common ports to scan
const uint16_t commonPorts[] = {
  21, 22, 23, 25, 53, 80, 110, 139, 143, 443, 445, 3389, 8080, 8443
};
#define PORTS_COUNT (sizeof(commonPorts)/sizeof(uint16_t))

inline void drawPortScanScreen() {
  tft.fillScreen(COLOR_BG);
  drawHeader("Port Scanner", true);

  if (WiFi.status() != WL_CONNECTED) {
    drawPanel(20, 40, 280, 60);
    tft.setTextColor(COLOR_WARN);
    tft.setCursor(30, 60);
    tft.print("Conecte ao Wi-Fi primeiro");
    drawStatusBar("Longo:sair  BACK:voltar");
    return;
  }

  drawPanel(20, 30, 280, 30);
  tft.setTextColor(COLOR_TEXT);
  tft.setCursor(30, 40);
  tft.printf("Alvo: %s", portScanTarget);

  // Results area
  int startY = 70;
  tft.setTextColor(COLOR_WARN);
  tft.setCursor(10, startY);
  tft.printf("Portas abertas (%d):", portScanCurrent);

  int y = startY + 16;
  for (int i = 0; i < portScanCurrent && i < 10; i++) {
    if (portResults[i].open) {
      tft.setTextColor(COLOR_PRIMARY);
      tft.setCursor(20, y);
      tft.printf("Port %5d OPEN", portResults[i].port);
      y += 14;
    }
  }

  if (portScanActive) {
    tft.setTextColor(COLOR_WARN);
    tft.setCursor(10, SCREEN_H - 28);
    tft.printf("Escaneando... %d/%d", portScanCurrent, portScanTotal);
  } else {
    tft.setTextColor(COLOR_TEXT);
    tft.setCursor(10, SCREEN_H - 28);
    tft.print("Longo:iniciar  BACK:sair");
  }
}

inline void enterWifiPortScan() {
  currentState = STATE_WIFI_PORTSCAN;
  portScanActive = false;
  portScanCurrent = 0;
  portScanTotal = PORTS_COUNT;
  for (int i = 0; i < 16; i++) portResults[i].port = 0;
  // Default target: gateway
  IPAddress gw = WiFi.gatewayIP();
  snprintf(portScanTarget, sizeof(portScanTarget), "%d.%d.%d.%d", gw[0], gw[1], gw[2], gw[3]);
  drawPortScanScreen();
}

inline void startPortScan() {
  if (WiFi.status() != WL_CONNECTED) return;
  portScanActive = true;
  portScanCurrent = 0;
  WiFiClient client;
  IPAddress target;
  if (!target.fromString(portScanTarget)) {
    // default to gateway
    target = WiFi.gatewayIP();
  }

  for (int i = 0; i < PORTS_COUNT && portScanActive; i++) {
    client.stop();
    bool ok = client.connect(target, commonPorts[i], 200);  // 200ms timeout
    portResults[i].port = commonPorts[i];
    portResults[i].open = ok;
    portScanCurrent = i + 1;
    if (i % 3 == 0) drawPortScanScreen();
    delay(10);
  }
  portScanActive = false;
  drawPortScanScreen();
}
