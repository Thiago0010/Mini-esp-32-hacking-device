/*
 * ============================================================================
 * [NEV]² - NEV-OS CyberLab v3.0
 * module_badusb.h - BadUSB demo (STUB DIDATICO)
 *
 * ============================================================================
 * ATENCAO IMPORTANTE: ESP32-WROOM nao tem USB device!
 * ============================================================================
 * O ESP32-WROOM (e variantes standard) nao possui USB OTG/device.
 * Para BadUSB real, voce precisa de:
 *  - ESP32-S2 (com USB nativo)
 *  - ESP32-S3 (com USB nativo + OTG)
 *
 * Esses chips suportam a lib "Adafruit TinyUSB Library" que
 * implementa USB HID device.
 *
 * Este modulo existe apenas para fins EDUCACIONAIS, mostrando
 * a um aluno:
 *   1. Por que BadUSB funciona (USB HID aceito por OS)
 *   2. Como seria o payload (DuckEncoder-like)
 *   3. Limitacoes do hardware
 *
 * NAO tenta executar payload real.
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

const char BADUSB_INFO[] = R"raw(
ESP32-WROOM NAO tem USB device.

Para BadUSB real, use:
  - ESP32-S2  (USB nativo)
  - ESP32-S3  (USB nativo)

Lib necessaria:
  Adafruit TinyUSB Library

Exemplo de payload (nao executa):
  DELAY 1000
  GUI r
  DELAY 200
  STRING notepad
  ENTER
  DELAY 500
  STRING Hello from NEV-OS
)raw";

inline void drawBadUsbScreen() {
  tft.fillScreen(COLOR_BG);
  drawHeader("BadUSB [demo]", true);

  drawInfoBox(
    "STUB - APENAS EDUCACIONAL",
    BADUSB_INFO,
    COLOR_DANGER
  );

  tft.setCursor(20, 215);
  tft.setTextColor(COLOR_TEXT);
  tft.setTextSize(1);
  tft.print("Migre para ESP32-S3 para BadUSB real");
  drawStatusBar("Longo:sair  BACK:voltar");
}

inline void enterBadUsb() {
  currentState = STATE_BADUSB;
  drawBadUsbScreen();
}
