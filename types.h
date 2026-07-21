/*
 * ============================================================================
 * [NEV]² - NEV-OS CyberLab v3.0
 * types.h - Core data structures and enums
 * ============================================================================
 */
#pragma once

#include <Arduino.h>
#include <WiFi.h>

// Forward declaration of MenuItem (recursive: children point to MenuItem[])
struct MenuItem;

// ============================================================================
// NETWORK (Wi-Fi scan result)
// ============================================================================
struct Network {
  char ssid[33];
  uint8_t ch;
  int rssi;
  wifi_auth_mode_t enc;
  uint8_t bssid[6];
};

// ============================================================================
// BLE DEVICE
// ============================================================================
struct BleDevice {
  char name[32];
  char addr[18];
  int rssi;
  uint8_t addrBytes[6];
  bool hasName;
};

// ============================================================================
// PORT SCAN RESULT
// ============================================================================
struct PortResult {
  uint16_t port;
  bool open;
};

// ============================================================================
// MENU ITEM
// ============================================================================
struct MenuItem {
  const char* label;
  void (*onEnter)();           // called on long press
  const MenuItem* children;    // submenu, or nullptr if leaf
  uint8_t childCount;
  uint8_t iconId;              // bitmap index in assets.h (future use)
};

// ============================================================================
// MENU NAVIGATION
// ============================================================================
#define MAX_NAV_DEPTH  4
struct MenuNav {
  const MenuItem* stack[MAX_NAV_DEPTH];
  uint8_t stackDepth;
  uint8_t selectedIndex[MAX_NAV_DEPTH];
  // path breadcrumb (read-only, for header)
  char currentTitle[24];
};

// ============================================================================
// APP STATE (kept for legacy + dispatch)
// ============================================================================
enum AppState {
  STATE_MENU = 0,
  STATE_IR,
  STATE_IR_REPLAY,
  STATE_WIFI_SCAN,
  STATE_WIFI_DEAUTH,
  STATE_WIFI_CAPTIVE,
  STATE_WIFI_EVIL,
  STATE_WIFI_BEACON,
  STATE_WIFI_DETECTOR,
  STATE_WIFI_PING,
  STATE_WIFI_PORTSCAN,
  STATE_WIFI_FTP,
  STATE_WIFI_OTA,
  STATE_BLE_SCAN,
  STATE_BLE_SPAM,
  STATE_BLE_ADV,
  STATE_BLE_SNIFF,
  STATE_BLE_ATTACK,
  STATE_BLE_MOUSE,
  STATE_BLE_KBD,
  STATE_BADUSB,
  STATE_SD,
  STATE_GAME,
  STATE_SETTINGS,
  STATE_REBOOT,
  STATE_COUNT
};

// ============================================================================
// TOUCH STATE (shared with touch_driver.h)
// ============================================================================
struct TouchState {
  bool isTouching;
  bool actionHandled;
  unsigned long startTime;
  int lastX;
  int lastY;
};

// ============================================================================
// CYBER PONG
// ============================================================================
struct PongState {
  int paddleX;
  float ballX, ballY;
  float ballVX, ballVY;
  int score;
  int misses;
  int prevPaddleX;
  float prevBallX, prevBallY;
};

// ============================================================================
// IR STATE
// ============================================================================
struct IrState {
  char protocol[16];
  char value[24];
  int bits;
  int count;
  // Replay support
  uint64_t savedValue;
  char savedProtocol[16];
  int savedBits;
  bool hasSaved;
};
