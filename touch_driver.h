/*
 * ============================================================================
 * [NEV]² - NEV-OS CyberLab v3.0
 * touch_driver.h - Bit-bang XPT2046 touch driver
 *
 * ATENÇÃO: A lógica de leitura bit-bang foi copiada EXATAMENTE do código
 * original. NÃO ALTERAR. Função sensível: foi ajustada empiricamente.
 * ============================================================================
 */
#pragma once

#include <Arduino.h>
#include <TFT_eSPI.h>
#include "config.h"

extern TFT_eSPI tft;
extern TouchState touchState;

// ============================================================================
// BIT-BANG TOUCH READ (ISOLADO - NÃO MEXER)
// ============================================================================
inline void lerTouchTotalmenteIsolado(int &x, int &y, int &z) {
  digitalWrite(TOUCH_CS, LOW);
  uint8_t cmds[] = {0xB0, 0xD0, 0x90};
  int *res[] = {&z, &x, &y};
  for (int c = 0; c < 3; c++) {
    uint8_t cmd = cmds[c];
    for (int i = 0; i < 8; i++) {
      digitalWrite(TOUCH_MOSI, (cmd & 0x80) ? HIGH : LOW); cmd <<= 1;
      digitalWrite(TOUCH_CLK, HIGH); digitalWrite(TOUCH_CLK, LOW);
    }
    int val = 0;
    for (int i = 0; i < 12; i++) {
      digitalWrite(TOUCH_CLK, HIGH); val <<= 1; if (digitalRead(TOUCH_MISO)) val |= 1; digitalWrite(TOUCH_CLK, LOW);
    }
    *res[c] = val;
  }
  digitalWrite(TOUCH_CS, HIGH);
}

// ============================================================================
// TOUCH HANDLER
// Lógica idêntica ao código original: debounce, short press vs long press.
// ============================================================================
typedef void (*ShortPressCallback)();
typedef void (*LongPressCallback)();

inline void handleTouchGeneric(ShortPressCallback onShort, LongPressCallback onLong) {
  int tx = 0, ty = 0, tz = 0;
  lerTouchTotalmenteIsolado(tx, ty, tz);

  static byte touchCounter = 0;
  bool debouncedTouch = false;

  if (tz > TOUCH_THRESHOLD) {
    touchCounter++;
    if (touchCounter >= TOUCH_DEBOUNCE) {
      debouncedTouch = true;
      touchCounter = TOUCH_DEBOUNCE;
    }
  } else {
    touchCounter = 0;
  }

  if (debouncedTouch) {
    if (!touchState.isTouching) {
      touchState.isTouching = true;
      touchState.startTime = millis();
      touchState.actionHandled = false;
      touchState.lastX = tx;
      touchState.lastY = ty;
    } else {
      if ((millis() - touchState.startTime >= LONG_TOUCH_DELAY) && !touchState.actionHandled) {
        touchState.actionHandled = true;
        if (onLong) onLong();
      }
    }
  } else {
    if (touchState.isTouching) {
      unsigned long touchDuration = millis() - touchState.startTime;
      touchState.isTouching = false;

      if (touchDuration > 30 && touchDuration < LONG_TOUCH_DELAY && !touchState.actionHandled) {
        if (onShort) onShort();
      }
    }
  }
}

// Reads raw touch coords (screen space)
inline bool readTouchScreen(int &sx, int &sy) {
  int tx = 0, ty = 0, tz = 0;
  lerTouchTotalmenteIsolado(tx, ty, tz);
  if (tz > TOUCH_THRESHOLD) {
    sx = map(tx, TOUCH_MIN_X, TOUCH_MAX_X, 0, SCREEN_W);
    sy = map(ty, TOUCH_MIN_Y, TOUCH_MAX_Y, 0, SCREEN_H);
    sx = constrain(sx, 0, SCREEN_W);
    sy = constrain(sy, 0, SCREEN_H);
    return true;
  }
  return false;
}

// ============================================================================
// ONE-SHOT TOUCH STATE INIT
// ============================================================================
inline void initTouch() {
  touchState.isTouching = false;
  touchState.actionHandled = false;
  touchState.startTime = 0;
  touchState.lastX = 0;
  touchState.lastY = 0;
}
