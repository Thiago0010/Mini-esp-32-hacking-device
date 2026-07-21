/*
 * ============================================================================
 * [NEV]² - NEV-OS CyberLab v3.0
 * module_ble_mouse.h - BLE Mouse (HID)
 *
 * STATUS: EXPERIMENTAL - Requer NimBLE-Mouse-Keyboard lib OU implementação
 * direta do HID over GATT. Em ESP32-WROOM, BLE HID mouse tem suporte
 * limitado em hosts modernos (iOS 17+, Android 13+).
 *
 * Para funcionamento real:
 * 1. Instalar lib "NimBLE-Mouse-Keyboard" via Library Manager
 * 2. Ou implementar HID Mouse descriptor manualmente
 * 3. Parear com host (PIN geralmente 0000)
 * ============================================================================
 */
#pragma once

#include <Arduino.h>
#include <TFT_eSPI.h>
#include "config.h"
#include "types.h"
#include "ui_helpers.h"

extern TFT_eSPI tft;
extern AppState currentState;

inline void drawBleMouseScreen() {
  tft.fillScreen(COLOR_BG);
  drawHeader("BLE Mouse [experimental]", true);

  drawInfoBox(
    "BLE HID Mouse",
    "Requer lib 'NimBLE-Mouse-Keyboard' (h2zero) instalada no Arduino IDE.\n"
    "\n"
    "Limitacoes conhecidas:\n"
    "- iOS 17+ bloqueia HID nao pareado\n"
    "- Windows 10/11 funciona apos parear\n"
    "- Android 13+ exige permissao LOCAL_DEVICE_NAME\n"
    "\n"
    "Para parear:\n"
    "1. Host procura 'NEV-Mouse'\n"
    "2. PIN padrao: 0000\n"
    "3. Toque longo no ESP32 para mover cursor",
    COLOR_WARN
  );

  tft.setCursor(20, 215);
  tft.setTextColor(COLOR_TEXT);
  tft.setTextSize(1);
  tft.print("TODO: implementar HID descriptor");
  drawStatusBar("Longo:sair  BACK:voltar");
}

inline void enterBleMouse() {
  currentState = STATE_BLE_MOUSE;
  drawBleMouseScreen();
}

/*
// =============================================================================
// EXEMPLO DE IMPLEMENTACAO REAL (descomentar apos instalar lib)
// =============================================================================
#include <NimBLEMouse.h>
NimBLEMouse bleMouse("NEV-Mouse", "NEV-OS", 100);

void enterBleMouseReal() {
  NimBLEDevice::init("NEV-Mouse");
  bleMouse.begin();
  // Espera parear
  // ...
}

void handleBleMouseMove(int dx, int dy, uint8_t buttons) {
  if (bleMouse.isConnected()) {
    bleMouse.move(dx, dy, 0, buttons);
  }
}
*/
