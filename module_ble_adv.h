/*
 * ============================================================================
 * [NEV]² - NEV-OS CyberLab v3.0
 * module_ble_adv.h - Custom BLE Advertiser
 * ============================================================================
 */
#pragma once

#include <Arduino.h>
#include <NimBLEDevice.h>
#include <TFT_eSPI.h>
#include "config.h"
#include "types.h"
#include "ui_helpers.h"

extern TFT_eSPI tft;
extern AppState currentState;
extern bool bleAdvActive;
extern char bleAdvName[20];

inline void drawBleAdvScreen() {
  tft.fillScreen(COLOR_BG);
  drawHeader("BLE Advertise", true);

  drawPanel(20, 30, 280, 80);
  tft.setTextColor(COLOR_TEXT);
  tft.setCursor(30, 42);
  tft.print("Nome:");
  tft.setTextColor(COLOR_ACCENT);
  tft.setCursor(70, 42);
  tft.print(bleAdvName);

  tft.setTextColor(COLOR_TEXT);
  tft.setCursor(30, 62);
  tft.print("Status:");
  tft.setTextColor(bleAdvActive ? COLOR_DANGER : COLOR_PRIMARY);
  tft.setCursor(80, 62);
  tft.print(bleAdvActive ? "BROADCASTING" : "IDLE");

  tft.setTextColor(COLOR_TEXT);
  tft.setCursor(30, 82);
  tft.print("MAC:");
  tft.setTextColor(COLOR_WARN);
  tft.setCursor(70, 82);
  if (bleAdvActive) tft.print(NimBLEDevice::getAddress().toString().c_str());
  else tft.print("(nao iniciado)");

  int btnY = 130;
  tft.fillRoundRect(60, btnY, 200, 40, 8, bleAdvActive ? 0x4208 : COLOR_PANEL_BG);
  tft.drawRoundRect(60, btnY, 200, 40, 8, bleAdvActive ? COLOR_DANGER : COLOR_PRIMARY);
  drawCenteredText(bleAdvActive ? "PARAR" : "BROADCAST", btnY + 14,
                   bleAdvActive ? COLOR_DANGER : COLOR_PRIMARY, 2);
  drawStatusBar("Longo:toggle  BACK:sair");
}

inline void enterBleAdv() {
  currentState = STATE_BLE_ADV;
  bleAdvActive = false;
  strncpy(bleAdvName, "NEV-OS", sizeof(bleAdvName)-1);
  drawBleAdvScreen();
}

inline void toggleBleAdv() {
  bleAdvActive = !bleAdvActive;
  if (bleAdvActive) {
    NimBLEDevice::init(bleAdvName);
    NimBLEAdvertising* pAdv = NimBLEDevice::getAdvertising();
    pAdv->stop();
    pAdv->setAdvertisementType(BLE_HS_ADV_TYPE_ADV_IND);
    pAdv->setMinInterval(100);
    pAdv->setMaxInterval(200);
    pAdv->setName(String(bleAdvName).c_str());
    pAdv->start();
  } else {
    NimBLEDevice::getAdvertising()->stop();
    NimBLEDevice::deinit(true);
  }
  drawBleAdvScreen();
}
