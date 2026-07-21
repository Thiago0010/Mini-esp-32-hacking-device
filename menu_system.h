/*
 * ============================================================================
 * [NEV]² - NEV-OS CyberLab v3.0
 * menu_system.h - Generic tree-based menu with navigation stack
 * ============================================================================
 */
#pragma once

#include <Arduino.h>
#include <TFT_eSPI.h>
#include "config.h"
#include "types.h"
#include "ui_helpers.h"

extern TFT_eSPI tft;
extern MenuNav nav;
extern AppState currentState;

// ============================================================================
// FORWARD DECLARATIONS (menu items call these)
// ============================================================================
void enterWifiMenu();
void enterBleMenu();
void enterMiscMenu();
void enterSettingsMenu();
void enterIrReceive();
void enterIrReplay();
void enterSdBrowser();
void enterCyberPong();
void enterWifiScan();
void enterWifiDeauth();
void enterWifiCaptive();
void enterWifiEvil();
void enterWifiBeacon();
void enterWifiDetector();
void enterWifiPing();
void enterWifiPortScan();
void enterWifiFtp();
void enterWifiOta();
void enterBleScan();
void enterBleSpam();
void enterBleAdv();
void enterBleSniff();
void enterBleAttack();
void enterBleMouse();
void enterBleKbd();
void enterBadUsb();

// ============================================================================
// MENU TREE - PROGMEM-like constants (using const, lives in flash)
// ============================================================================

// Wi-Fi submenu
const MenuItem wifiChildren[] = {
  {"Wi-Fi Scanner",   enterWifiScan,      nullptr, 0, 0},
  {"Deauth Attack",   enterWifiDeauth,    nullptr, 0, 0},
  {"Captive Portal",  enterWifiCaptive,   nullptr, 0, 0},
  {"Evil Portal",     enterWifiEvil,      nullptr, 0, 0},
  {"Beacon Spam",     enterWifiBeacon,    nullptr, 0, 0},
  {"Wi-Fi Detector",  enterWifiDetector,  nullptr, 0, 0},
  {"Ping Scanner",    enterWifiPing,      nullptr, 0, 0},
  {"Port Scanner",    enterWifiPortScan,  nullptr, 0, 0},
  {"FTP Client",      enterWifiFtp,       nullptr, 0, 0},
  {"OTA Update",      enterWifiOta,       nullptr, 0, 0},
};

// BLE submenu
const MenuItem bleChildren[] = {
  {"BLE Scanner",     enterBleScan,       nullptr, 0, 0},
  {"BLE Spam",        enterBleSpam,       nullptr, 0, 0},
  {"BLE Advertise",   enterBleAdv,        nullptr, 0, 0},
  {"BLE Sniffer",     enterBleSniff,      nullptr, 0, 0},
  {"BLE Attack",      enterBleAttack,     nullptr, 0, 0},
  {"BLE Mouse (HID)", enterBleMouse,      nullptr, 0, 0},
  {"BLE Keyboard",    enterBleKbd,        nullptr, 0, 0},
};

// Misc submenu
const MenuItem miscChildren[] = {
  {"IR Receive",      enterIrReceive,     nullptr, 0, 0},
  {"IR Replay",       enterIrReplay,      nullptr, 0, 0},
  {"SD Browser",      enterSdBrowser,     nullptr, 0, 0},
  {"Cyber Pong",      enterCyberPong,     nullptr, 0, 0},
  {"BadUSB [demo]",   enterBadUsb,        nullptr, 0, 0},
};

// Settings submenu
const MenuItem settingsChildren[] = {
  {"Wi-Fi Config",    enterWifiOta,       nullptr, 0, 0},  // placeholder
  {"About",           enterBadUsb,        nullptr, 0, 0},  // placeholder
  {"Reboot",          enterCyberPong,     nullptr, 0, 0},  // placeholder
};

// Root menu
const MenuItem rootMenu[] = {
  {"Wi-Fi Tools",     enterWifiMenu,      wifiChildren,    sizeof(wifiChildren)/sizeof(MenuItem),    0},
  {"BLE Tools",       enterBleMenu,       bleChildren,     sizeof(bleChildren)/sizeof(MenuItem),     0},
  {"Misc & Demo",     enterMiscMenu,      miscChildren,    sizeof(miscChildren)/sizeof(MenuItem),    0},
  {"Settings",        enterSettingsMenu,  settingsChildren,sizeof(settingsChildren)/sizeof(MenuItem),0},
};

#define ROOT_MENU_COUNT  (sizeof(rootMenu)/sizeof(MenuItem))

// ============================================================================
// NAVIGATION CORE
// ============================================================================
inline void menuInit() {
  nav.stack[0] = rootMenu;
  nav.stackDepth = 0;
  for (int i = 0; i < MAX_NAV_DEPTH; i++) nav.selectedIndex[i] = 0;
  strcpy(nav.currentTitle, "Main Panel");
  currentState = STATE_MENU;
}

// Push current selection's children as new level
inline void menuEnter() {
  const MenuItem* current = nav.stack[nav.stackDepth];
  uint8_t sel = nav.selectedIndex[nav.stackDepth];

  if (sel >= current[0].childCount && current[0].childCount > 0) {
    // selecting a parent; go into its children
    if (nav.stackDepth + 1 < MAX_NAV_DEPTH) {
      nav.stackDepth++;
      nav.stack[nav.stackDepth] = current[sel].children;
      nav.selectedIndex[nav.stackDepth] = 0;
      strncpy(nav.currentTitle, current[sel].label, sizeof(nav.currentTitle)-1);
      nav.currentTitle[sizeof(nav.currentTitle)-1] = '\0';
    }
  } else {
    // leaf: call onEnter
    if (current[sel].onEnter) current[sel].onEnter();
  }
}

// Advance selection with wrap
inline void menuNext() {
  const MenuItem* current = nav.stack[nav.stackDepth];
  uint8_t count = (current == rootMenu) ? ROOT_MENU_COUNT : current[0].childCount;
  if (count == 0) return;
  nav.selectedIndex[nav.stackDepth] = (nav.selectedIndex[nav.stackDepth] + 1) % count;
}

// Pop one level
inline void menuBack() {
  if (nav.stackDepth == 0) return;
  nav.stackDepth--;
  strcpy(nav.currentTitle, "Main Panel");
}

// ============================================================================
// RENDER: Generic list of MenuItem[]
// ============================================================================
inline void drawMenuList() {
  tft.fillScreen(COLOR_BG);
  drawHeader(nav.currentTitle, nav.stackDepth > 0);

  const MenuItem* items;
  uint8_t count;
  if (nav.stack[nav.stackDepth] == rootMenu) {
    items = rootMenu;
    count = ROOT_MENU_COUNT;
  } else {
    // current level's parent is one level up; current level is stack[depth] which is the children[]
    // We need to read the count from somewhere; the first item in children[] may not have childCount set.
    // Easier: ask the parent about its childCount and use that count.
    const MenuItem* parent = nav.stack[nav.stackDepth - 1];
    uint8_t sel = nav.selectedIndex[nav.stackDepth - 1];
    count = parent[sel].childCount;
    items = nav.stack[nav.stackDepth];
  }
  uint8_t selected = nav.selectedIndex[nav.stackDepth];

  // Each item is 50px tall, max 4 visible (240-20-20 = 200 usable, /50 = 4)
  int startY = HEADER_H + 10;
  int itemH = 50;
  int maxVisible = 4;
  int startIdx = 0;
  if (selected >= maxVisible) startIdx = selected - maxVisible + 1;

  for (int i = 0; i < maxVisible && (startIdx + i) < count; i++) {
    int idx = startIdx + i;
    int y = startY + i * itemH;
    bool isSel = (idx == selected);

    if (isSel) {
      tft.fillRoundRect(ITEM_X, y, ITEM_W, itemH - 4, 6, COLOR_PANEL_BG);
      tft.drawRoundRect(ITEM_X, y, ITEM_W, itemH - 4, 6, COLOR_WARN);
      tft.setTextColor(COLOR_WARN);
    } else {
      tft.drawRoundRect(ITEM_X, y, ITEM_W, itemH - 4, 6, COLOR_BORDER);
      tft.setTextColor(COLOR_TEXT);
    }
    tft.setTextSize(2);
    tft.setCursor(ITEM_X + 14, y + 16);
    tft.print(items[idx].label);

    if (items[idx].childCount > 0) {
      tft.setTextColor(COLOR_PRIMARY);
      tft.setTextSize(2);
      tft.setCursor(ITEM_X + ITEM_W - 24, y + 16);
      tft.print(">");
    }
  }

  // Hint footer
  tft.setTextColor(COLOR_TEXT);
  tft.setTextSize(1);
  tft.setCursor(6, SCREEN_H - 10);
  tft.print("Curto:Proximo  Longo:Entra  Longo no BACK:Volta");
}

// ============================================================================
// BOOT SPLASH
// ============================================================================
inline void showBootIntro() {
  tft.fillScreen(COLOR_BG);
  tft.fillRect(0, 80, SCREEN_W, 80, COLOR_PANEL_BG);
  tft.setTextColor(COLOR_ACCENT);
  tft.setTextSize(2);
  tft.setCursor(40, 100);
  tft.print("[NEV]2  NEV-OS");
  tft.setTextSize(1);
  tft.setTextColor(COLOR_TEXT);
  tft.setCursor(70, 130);
  tft.print("CyberLab v3.0");
  tft.setCursor(50, 150);
  tft.print("Inicializando...");
  delay(1500);
}
