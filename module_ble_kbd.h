/*
 * ============================================================================
 * [NEV]² - NEV-OS CyberLab v3.0
 * module_ble_kbd.h - BLE Keyboard (HID)
 *
 * STATUS: EXPERIMENTAL - Mesmas limitações do BLE Mouse.
 * Para uso real, instale a lib "NimBLE-Mouse-Keyboard".
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

inline void drawBleKbdScreen() {
  tft.fillScreen(COLOR_BG);
  drawHeader("BLE Keyboard [experimental]", true);

  drawInfoBox(
    "BLE HID Keyboard",
    "Requer lib 'NimBLE-Mouse-Keyboard' (h2zero).\n"
    "\n"
    "Limitacoes:\n"
    "- Hosts modernos exigem pareamento manual\n"
    "- Muitos hosts exibem alerta de seguranca\n"
    "- USB OTG + HID eh muito mais confiavel\n"
    "\n"
    "Exemplo de uso (apos lib instalada):\n"
    "  bleKbd.begin();\n"
    "  bleKbd.print(\"Hello from NEV-OS\");\n"
    "  bleKbd.press(KEY_RETURN);\n"
    "  bleKbd.releaseAll();\n"
    "\n"
    "Para automacao real, use:\n"
    "- ESP32-S2/S3 com USB HID (ver module_badusb.h)\n"
    "- Ou USB OTG com adaptador",
    COLOR_WARN
  );

  tft.setCursor(20, 215);
  tft.setTextColor(COLOR_TEXT);
  tft.setTextSize(1);
  tft.print("TODO: implementar HID keyboard");
  drawStatusBar("Longo:sair  BACK:voltar");
}

inline void enterBleKbd() {
  currentState = STATE_BLE_KBD;
  drawBleKbdScreen();
}
