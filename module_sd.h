/*
 * ============================================================================
 * [NEV]² - NEV-OS CyberLab v3.0
 * module_sd.h - SD card browser
 * ============================================================================
 */
#pragma once

#include <Arduino.h>
#include <SD.h>
#include <TFT_eSPI.h>
#include "config.h"
#include "types.h"
#include "ui_helpers.h"

extern TFT_eSPI tft;
extern AppState currentState;
extern bool sdCardReady;
extern unsigned long sdCardSizeMB;
extern unsigned long sdUsedMB;
extern char sdFileList[MAX_SD_FILES][SD_FILE_NAME_LEN];
extern int sdFileCount;
extern int sdPage;

inline void initSd() {
  digitalWrite(TFT_CS, HIGH);
  sdCardReady = SD.begin(SD_CS, SPI);
  digitalWrite(TFT_CS, HIGH);
}

inline void updateSdInfo() {
  digitalWrite(TFT_CS, HIGH);
  sdCardReady = SD.begin(SD_CS, SPI);
  digitalWrite(TFT_CS, HIGH);

  sdFileCount = 0;
  sdCardSizeMB = 0;
  sdUsedMB = 0;

  if (!sdCardReady) return;

  uint8_t cardType = SD.cardType();
  if (cardType == CARD_NONE) {
    sdCardReady = false;
    return;
  }

  sdCardSizeMB = (unsigned long)(SD.cardSize() / (1024ULL * 1024ULL));

  uint64_t usedBytes = 0;
  File root = SD.open("/");
  if (root) {
    File entry = root.openNextFile();
    while (entry) {
      if (!entry.isDirectory()) {
        usedBytes += entry.size();
        if (sdFileCount < MAX_SD_FILES) {
          strncpy(sdFileList[sdFileCount], entry.name(), SD_FILE_NAME_LEN-1);
          sdFileList[sdFileCount][SD_FILE_NAME_LEN-1] = '\0';
          sdFileCount++;
        }
      }
      entry.close();
      entry = root.openNextFile();
    }
    root.close();
  }
  sdUsedMB = (unsigned long)(usedBytes / (1024ULL * 1024ULL));
}

inline void drawSdScreen() {
  tft.fillScreen(COLOR_BG);
  drawHeader("SD Browser", true);

  if (!sdCardReady) {
    drawPanel(20, 40, 280, 80);
    tft.setTextColor(COLOR_DANGER);
    tft.setCursor(30, 60);
    tft.print("SD nao detectado!");
    tft.setCursor(30, 80);
    tft.print("Verifique o modulo e o CS (GPIO 15).");
    drawStatusBar("Longo:rescan  BACK:voltar");
    return;
  }

  drawPanel(20, 30, 280, 50);
  tft.setTextColor(COLOR_TEXT);
  tft.setCursor(30, 42);
  tft.printf("Total: %lu MB", sdCardSizeMB);
  tft.setCursor(30, 62);
  tft.printf("Usado: %lu MB (%lu%%)",
             sdUsedMB,
             sdCardSizeMB ? (sdUsedMB * 100 / sdCardSizeMB) : 0);

  // File list
  int startY = 95;
  tft.setTextColor(COLOR_PRIMARY);
  tft.setCursor(10, startY);
  tft.printf("Arquivos (%d):", sdFileCount);

  int y = startY + 16;
  for (int i = 0; i < sdFileCount && i < 8; i++) {
    tft.setTextColor(COLOR_TEXT);
    tft.setCursor(20, y);
    tft.print(sdFileList[i]);
    y += 14;
  }

  drawStatusBar("Longo:rescan  BACK:voltar");
}

inline void enterSdBrowser() {
  currentState = STATE_SD;
  updateSdInfo();
  drawSdScreen();
}
