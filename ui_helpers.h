/*
 * ============================================================================
 * [NEV]² - NEV-OS CyberLab v3.0
 * ui_helpers.h - Reusable UI primitives
 * ============================================================================
 */
#pragma once

#include <Arduino.h>
#include <TFT_eSPI.h>
#include "config.h"

extern TFT_eSPI tft;

// ============================================================================
// HEADER
// ============================================================================
inline void drawHeader(const char* title, bool showBack = true) {
  tft.fillRect(0, 0, SCREEN_W, HEADER_H, COLOR_HEADER);
  tft.drawLine(0, HEADER_H, SCREEN_W, HEADER_H, COLOR_PRIMARY);
  tft.setTextColor(COLOR_ACCENT);
  tft.setTextSize(1);
  tft.setCursor(6, 6);
  tft.print(title);
  if (showBack) {
    tft.drawRoundRect(BACK_BTN_X, BACK_BTN_Y, BACK_BTN_W, BACK_BTN_H, 4, COLOR_WARN);
    tft.setTextColor(COLOR_WARN);
    tft.setCursor(BACK_BTN_X + 14, BACK_BTN_Y + 6);
    tft.print("< BACK");
  }
}

// ============================================================================
// STATUS BAR (footer)
// ============================================================================
inline void drawStatusBar(const char* text) {
  tft.fillRect(0, SCREEN_H - 16, SCREEN_W, 16, COLOR_BG);
  tft.drawLine(0, SCREEN_H - 16, SCREEN_W, SCREEN_H - 16, COLOR_BORDER);
  tft.setTextColor(COLOR_TEXT);
  tft.setTextSize(1);
  tft.setCursor(6, SCREEN_H - 12);
  tft.print(text);
}

// ============================================================================
// PANEL (rounded rect with border)
// ============================================================================
inline void drawPanel(int x, int y, int w, int h, uint16_t bg = COLOR_PANEL_BG, uint16_t border = COLOR_PANEL_BD) {
  tft.fillRoundRect(x, y, w, h, 8, bg);
  tft.drawRoundRect(x, y, w, h, 8, border);
}

// ============================================================================
// CENTERED TEXT
// ============================================================================
inline void drawCenteredText(const char* text, int y, uint16_t color = COLOR_TEXT, uint8_t size = 1) {
  tft.setTextSize(size);
  tft.setTextColor(color);
  int len = strlen(text);
  int charW = 6 * size;
  int x = (SCREEN_W - len * charW) / 2;
  if (x < 0) x = 0;
  tft.setCursor(x, y);
  tft.print(text);
}

// ============================================================================
// WRAPPED TEXT (multi-line, simple word wrap)
// ============================================================================
inline void drawWrappedText(const char* text, int x, int y, int maxW, uint16_t color = COLOR_TEXT, uint8_t size = 1) {
  tft.setTextSize(size);
  tft.setTextColor(color);
  int charW = 6 * size;
  int maxChars = maxW / charW;
  int lineH = 8 * size;
  int len = strlen(text);
  int pos = 0;
  int lineY = y;
  while (pos < len) {
    int endPos = pos + maxChars;
    if (endPos >= len) endPos = len;
    else {
      while (endPos > pos && text[endPos] != ' ' && text[endPos] != '\n') endPos--;
      if (endPos == pos) endPos = pos + maxChars;
    }
    char buf[64];
    int copyLen = endPos - pos;
    if (copyLen >= (int)sizeof(buf)) copyLen = sizeof(buf) - 1;
    memcpy(buf, text + pos, copyLen);
    buf[copyLen] = '\0';
    tft.setCursor(x, lineY);
    tft.print(buf);
    lineY += lineH;
    pos = endPos;
    if (text[pos] == ' ' || text[pos] == '\n') pos++;
    if (lineY > SCREEN_H - 20) break;
  }
}

// ============================================================================
// CONFIRM DIALOG HELPER
// ============================================================================
inline void drawConfirmBox(const char* question, const char* yesLabel = "SIM", const char* noLabel = "NAO") {
  tft.fillRoundRect(40, 80, 240, 80, 8, COLOR_BG);
  tft.drawRoundRect(40, 80, 240, 80, 8, COLOR_WARN);
  drawCenteredText(question, 100, COLOR_WARN, 1);
  // YES button (left)
  tft.fillRoundRect(60, 130, 80, 24, 4, COLOR_PANEL_BG);
  tft.drawRoundRect(60, 130, 80, 24, 4, COLOR_PRIMARY);
  drawCenteredText(yesLabel, 138, COLOR_PRIMARY, 1);
  // NO button (right)
  tft.fillRoundRect(180, 130, 80, 24, 4, COLOR_PANEL_BG);
  tft.drawRoundRect(180, 130, 80, 24, 4, COLOR_DANGER);
  drawCenteredText(noLabel, 138, COLOR_DANGER, 1);
}

// ============================================================================
// INFO BOX
// ============================================================================
inline void drawInfoBox(const char* title, const char* body, uint16_t titleColor = COLOR_ACCENT) {
  drawPanel(20, 50, 280, 140);
  tft.setTextColor(titleColor);
  tft.setTextSize(1);
  tft.setCursor(30, 60);
  tft.print(title);
  tft.drawLine(30, 72, 290, 72, COLOR_BORDER);
  drawWrappedText(body, 30, 80, 250, COLOR_TEXT, 1);
}
