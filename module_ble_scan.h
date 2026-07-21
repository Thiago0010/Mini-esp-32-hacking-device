/*
 * ============================================================================
 * [NEV]² - NEV-OS CyberLab v3.0
 * module_ble_scan.h - BLE scanner (NimBLE)
 * ============================================================================
 */
#pragma once

#include <Arduino.h>
#include <NimBLEDevice.h>
#include <NimBLEScan.h>
#include <TFT_eSPI.h>
#include "config.h"
#include "types.h"
#include "ui_helpers.h"

extern TFT_eSPI tft;
extern AppState currentState;
extern BleDevice bleDevices[MAX_BLE_DEVICES];
extern int bleDeviceCount;
extern bool bleScanActive;
extern NimBLEScan* pBleScan;

class BleScanCallbacks : public NimBLEAdvertisedDeviceCallbacks {
  void onResult(NimBLEAdvertisedDevice* advertisedDevice) {
    if (bleDeviceCount >= MAX_BLE_DEVICES) return;
    BleDevice& d = bleDevices[bleDeviceCount];
    strncpy(d.addr, advertisedDevice->getAddress().toString().c_str(), sizeof(d.addr)-1);
    d.addr[sizeof(d.addr)-1] = '\0';
    if (advertisedDevice->haveName()) {
      strncpy(d.name, advertisedDevice->getName().c_str(), sizeof(d.name)-1);
      d.name[sizeof(d.name)-1] = '\0';
      d.hasName = true;
    } else {
      strcpy(d.name, "?");
      d.hasName = false;
    }
    d.rssi = advertisedDevice->getRSSI();
    memcpy(d.addrBytes, advertisedDevice->getAddress().getNative(), 6);
    bleDeviceCount++;
  }
};

inline void drawBleScanScreen() {
  tft.fillScreen(COLOR_BG);
  drawHeader("BLE Scanner", true);

  drawPanel(20, 30, 280, 30);
  tft.setTextColor(COLOR_TEXT);
  tft.setCursor(30, 40);
  tft.printf("Devices: %d / %d", bleDeviceCount, MAX_BLE_DEVICES);

  int startY = 70;
  int maxShow = min(bleDeviceCount, 9);
  for (int i = 0; i < maxShow; i++) {
    tft.setTextColor(COLOR_WARN);
    tft.setCursor(10, startY + i * 18);
    char buf[24];
    strncpy(buf, bleDevices[i].name, sizeof(buf)-1);
    buf[sizeof(buf)-1] = '\0';
    tft.print(buf);
    tft.setTextColor(COLOR_TEXT);
    tft.setCursor(160, startY + i * 18);
    tft.printf("%ddBm", bleDevices[i].rssi);
    tft.setCursor(220, startY + i * 18);
    tft.setTextSize(1);
    tft.print(bleDevices[i].addr);
  }

  drawStatusBar("Longo:scan 5s  BACK:sair");
}

inline void enterBleScan() {
  currentState = STATE_BLE_SCAN;
  bleScanActive = false;
  bleDeviceCount = 0;
  drawBleScanScreen();
}

inline void startBleScan() {
  if (bleScanActive) return;
  tft.fillScreen(COLOR_BG);
  drawHeader("BLE Scanner", true);
  drawCenteredText("Escaneando BLE...", 110, COLOR_WARN, 1);
  drawStatusBar("Aguarde 5 segundos");

  NimBLEDevice::init("");
  NimBLEDevice::setPower(ESP_PWR_LVL_P9);  // max power
  pBleScan = NimBLEDevice::getScan();
  pBleScan->setAdvertisedDeviceCallbacks(new BleScanCallbacks());
  pBleScan->setActiveScan(true);
  pBleScan->setInterval(100);
  pBleScan->setWindow(99);
  bleDeviceCount = 0;
  pBleScan->start(BLE_SCAN_DURATION, false);
  pBleScan->clearResults();
  bleScanActive = false;
  drawBleScanScreen();
}
