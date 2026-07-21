/*
 * ============================================================================
 * [NEV]² - NEV-OS CyberLab v3.0
 * module_wifi_ping.h - ICMP ping scanner
 * ============================================================================
 */
#pragma once

#include <Arduino.h>
#include <WiFi.h>
#include <ESP32Ping.h>
#include <TFT_eSPI.h>
#include "config.h"
#include "types.h"
#include "ui_helpers.h"

extern TFT_eSPI tft;
extern AppState currentState;
extern bool pingActive;
extern int pingCurrent;
extern int pingFound;
extern int pingTotal;

// Must be connected to a WiFi network for ping to work
inline void drawPingScreen() {
  tft.fillScreen(COLOR_BG);
  drawHeader("Ping Scanner", true);

  if (WiFi.status() != WL_CONNECTED) {
    drawPanel(20, 40, 280, 100);
    tft.setTextColor(COLOR_WARN);
    tft.setCursor(30, 60);
    tft.print("Conecte a uma rede Wi-Fi");
    tft.setCursor(30, 80);
    tft.print("antes de usar o ping.");
    tft.setCursor(30, 100);
    tft.print("(Settings -> Wi-Fi Config)");
    drawStatusBar("Longo:sair  BACK:voltar");
    return;
  }

  drawPanel(20, 30, 280, 50);
  tft.setTextColor(COLOR_TEXT);
  tft.setCursor(30, 42);
  tft.printf("Progresso: %d / %d", pingCurrent, pingTotal);
  tft.setCursor(30, 62);
  tft.printf("Hosts ativos: %d", pingFound);

  // Progress bar
  int barW = 260;
  int barX = 30;
  int barY = 90;
  tft.drawRect(barX, barY, barW, 12, COLOR_BORDER);
  if (pingTotal > 0) {
    int fillW = (barW * pingCurrent) / pingTotal;
    tft.fillRect(barX + 1, barY + 1, fillW - 2, 10, COLOR_PRIMARY);
  }

  drawPanel(20, 115, 280, 80);
  tft.setTextColor(COLOR_WARN);
  tft.setCursor(30, 125);
  tft.printf("IP Local: %s", WiFi.localIP().toString().c_str());
  tft.setCursor(30, 145);
  tft.printf("Subnet: %s", WiFi.subnetMask().toString().c_str());
  tft.setCursor(30, 165);
  tft.printf("Gateway: %s", WiFi.gatewayIP().toString().c_str());

  drawStatusBar("Longo:iniciar  BACK:sair");
}

inline void enterWifiPing() {
  currentState = STATE_WIFI_PING;
  pingActive = false;
  pingCurrent = 0;
  pingFound = 0;
  pingTotal = 254;  // /24 subnet
  drawPingScreen();
}

inline void startPingScan() {
  if (WiFi.status() != WL_CONNECTED) return;
  pingActive = true;
  pingCurrent = 0;
  pingFound = 0;

  // Build base IP from local IP
  IPAddress local = WiFi.localIP();
  IPAddress mask = WiFi.subnetMask();
  IPAddress gw = WiFi.gatewayIP();

  // Scan /24 around gateway
  uint8_t base[4] = {gw[0], gw[1], gw[2], 0};
  (void)local; (void)mask;

  for (int i = 1; i <= 254 && pingActive; i++) {
    IPAddress target(base[0], base[1], base[2], i);
    if (Ping.ping(target, 1) > 0) {
      pingFound++;
    }
    pingCurrent = i;
    if (i % 5 == 0) drawPingScreen();
    delay(2);  // breathing room
  }
  pingActive = false;
  drawPingScreen();
}
