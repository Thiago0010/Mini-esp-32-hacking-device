/*
 * ============================================================================
 * [NEV]² - NEV-OS CyberLab v3.0
 * module_ble_sniff.h - BLE sniffer (passive advertisement capture)
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
extern bool bleSniffActive;
extern int bleSniffCount;
extern char bleSniffLog[12][40];

class BleSniffCallbacks : public NimBLEAdvertisedDeviceCallbacks {
  void onResult(NimBLEAdvertisedDevice* advertisedDevice) {
    if (!bleSniffActive) return;
    int slot = bleSniffCount % 12;
    snprintf(bleSniffLog[slot], 40, "%02X:%02X:%02X | %s",
             advertisedDevice->getAddress().getNative()[3],
             advertisedDevice->getAddress().getNative()[4],
             advertisedDevice->getAddress().getNative()[5],
             advertisedDevice->haveName() ? advertisedDevice->getName().c_str() : "?");
    bleSniffCount++;
  }
};

inline void drawBleSniffScreen() {
  tft.fillScreen(COLOR_BG);
  drawHeader("BLE Sniffer", true);

  drawPanel(20, 30, 280, 30);
  tft.setTextColor(COLOR_TEXT);
  tft.setCursor(30, 40);
  tft.printf("Status:%s  Total:%d",
             bleSniffActive ? " CAPTURANDO" : " IDLE",
             bleSniffCount);

  int startY = 70;
  int show = min(bleSniffCount, 10);
  for (int i = 0; i < show; i++) {
    int slot = (bleSniffCount - 1 - i + 12) % 12;
    tft.setTextColor(i == 0 ? COLOR_WARN : COLOR_TEXT);
    tft.setCursor(10, startY + i * 14);
    tft.print(bleSniffLog[slot]);
  }
  drawStatusBar("Longo:toggle  BACK:sair");
}

inline void enterBleSniff() {
  currentState = STATE_BLE_SNIFF;
  bleSniffActive = false;
  bleSniffCount = 0;
  drawBleSniffScreen();
}

inline void toggleBleSniff() {
  bleSniffActive = !bleSniffActive;
  if (bleSniffActive) {
    NimBLEDevice::init("");
    NimBLEScan* pScan = NimBLEDevice::getScan();
    pScan->setAdvertisedDeviceCallbacks(new BleSniffCallbacks());
    pScan->setActiveScan(true);
    pScan->start(0, nullptr, true);  // continuous
  } else {
    NimBLEDevice::getScan()->stop();
    NimBLEDevice::deinit(true);
  }
  drawBleSniffScreen();
}
