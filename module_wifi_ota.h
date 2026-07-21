/*
 * ============================================================================
 * [NEV]² - NEV-OS CyberLab v3.0
 * module_wifi_ota.h - OTA (Over-The-Air) update via Arduino IDE
 * ============================================================================
 */
#pragma once

#include <Arduino.h>
#include <WiFi.h>
#include <ArduinoOTA.h>
#include <TFT_eSPI.h>
#include "config.h"
#include "types.h"
#include "ui_helpers.h"

extern TFT_eSPI tft;
extern AppState currentState;
extern bool otaInProgress;
extern int otaProgress;

inline void drawOtaScreen() {
  tft.fillScreen(COLOR_BG);
  drawHeader("OTA Update", true);

  if (WiFi.status() != WL_CONNECTED) {
    drawPanel(20, 40, 280, 100);
    tft.setTextColor(COLOR_WARN);
    tft.setCursor(30, 60);
    tft.print("Conecte ao Wi-Fi primeiro");
    tft.setCursor(30, 80);
    tft.print("O OTA usa a rede local.");
    tft.setCursor(30, 100);
    tft.print("Use Settings -> Wi-Fi Config");
    drawStatusBar("Longo:sair  BACK:voltar");
    return;
  }

  drawPanel(20, 30, 280, 130);

  tft.setTextColor(COLOR_PRIMARY);
  tft.setTextSize(1);
  tft.setCursor(30, 42);
  tft.printf("Hostname: %s.local", OTA_HOSTNAME);
  tft.setCursor(30, 60);
  tft.printf("IP: %s", WiFi.localIP().toString().c_str());
  tft.setCursor(30, 78);
  tft.printf("Status: %s", otaInProgress ? "UPLOAD..." : "AGUARDANDO");
  tft.setCursor(30, 96);
  tft.printf("Progresso: %d%%", otaProgress);

  int barW = 240;
  tft.drawRect(35, 110, barW, 12, COLOR_BORDER);
  tft.fillRect(36, 111, (barW - 2) * otaProgress / 100, 10, COLOR_PRIMARY);

  drawCenteredText("Use Arduino IDE > Tools > Port > Network", 175, COLOR_TEXT, 1);
  drawCenteredText("Selecione mainhck e faca Upload", 195, COLOR_TEXT, 1);
  drawStatusBar("Longo:sair  BACK:voltar");
}

inline void setupOta() {
  ArduinoOTA.setHostname(OTA_HOSTNAME);
  ArduinoOTA.setPassword(OTA_PASSWORD);

  ArduinoOTA.onStart([]() {
    otaInProgress = true;
    tft.fillScreen(COLOR_BG);
    tft.setTextColor(COLOR_WARN);
    tft.setTextSize(2);
    tft.setCursor(40, 100);
    tft.print("OTA: Uploading...");
  });

  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    otaProgress = (progress * 100) / total;
    tft.fillRect(36, 111, 240 * otaProgress / 100, 10, COLOR_PRIMARY);
  });

  ArduinoOTA.onEnd([]() {
    otaInProgress = false;
    tft.fillScreen(COLOR_BG);
    tft.setTextColor(COLOR_PRIMARY);
    tft.setTextSize(2);
    tft.setCursor(80, 100);
    tft.print("Reiniciando...");
  });

  ArduinoOTA.onError([](ota_error_t error) {
    tft.fillScreen(COLOR_BG);
    tft.setTextColor(COLOR_DANGER);
    tft.setTextSize(1);
    tft.setCursor(10, 100);
    tft.printf("OTA Error[%u]", error);
  });

  ArduinoOTA.begin();
}

inline void enterWifiOta() {
  currentState = STATE_WIFI_OTA;
  otaInProgress = false;
  otaProgress = 0;
  setupOta();
  drawOtaScreen();
}

inline void handleOtaLoop() {
  ArduinoOTA.handle();
}
