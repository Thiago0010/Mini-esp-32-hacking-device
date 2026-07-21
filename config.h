/*
 * ============================================================================
 * [NEV]² - NEV-OS CyberLab v3.0
 * config.h - Hardware pins, colors, system constants
 * ============================================================================
 */
#pragma once

// ============================================================================
// HARDWARE PINS
// ============================================================================
#define TFT_CS        5
#define TFT_DC        2
#define TFT_RST       -1   // tied to EN or not used

#define IR_RX_PIN     27   // IR receiver (TSOP1738 etc)
#define IR_TX_PIN     25   // IR LED transmitter (NOVO para replay/aprender)

#define TOUCH_CS      21
#define TOUCH_CLK     26
#define TOUCH_MISO    33
#define TOUCH_MOSI    32

#define SD_CS         15   // SD card shares SPI bus with TFT

// ============================================================================
// DISPLAY
// ============================================================================
#define SCREEN_W      320
#define SCREEN_H      240

// Touch calibration
#define TOUCH_MIN_X   300
#define TOUCH_MAX_X   3800
#define TOUCH_MIN_Y   300
#define TOUCH_MAX_Y   3800

// ============================================================================
// COLORS (RGB565)
// ============================================================================
#define COLOR_BG        TFT_BLACK
#define COLOR_PRIMARY   0x07E0   // green
#define COLOR_ACCENT    0x07FF   // cyan
#define COLOR_WARN      0xFFE0   // yellow
#define COLOR_DANGER    0xF800   // red
#define COLOR_TEXT      TFT_WHITE
#define COLOR_BORDER    0x18E3   // dark gray
#define COLOR_PANEL_BG  0x53EB   // panel fill
#define COLOR_PANEL_BD  0xCD68   // panel border
#define COLOR_HEADER    0x0825   // header strip

// ============================================================================
// TOUCH TIMING
// ============================================================================
#define LONG_TOUCH_DELAY  500
#define TOUCH_DEBOUNCE    2
#define TOUCH_THRESHOLD   400

// ============================================================================
// LAYOUT
// ============================================================================
#define BACK_BTN_X     240
#define BACK_BTN_Y     0
#define BACK_BTN_W     80
#define BACK_BTN_H     20

#define HEADER_H       20
#define ITEM_H         50
#define ITEM_X         10
#define ITEM_W         300

// ============================================================================
// SYSTEM LIMITS
// ============================================================================
#define MAX_NETWORKS         16
#define MAX_BLE_DEVICES      24
#define MAX_SD_FILES         8
#define SD_FILE_NAME_LEN     32
#define WIFI_SCAN_TIMEOUT_MS 5000

// ============================================================================
// OTA / NETWORKING
// ============================================================================
#define OTA_HOSTNAME         "mainhck"
#define OTA_PASSWORD         "nev12345"
#define WIFI_CONNECT_TIMEOUT 10000

// ============================================================================
// BLE / DEAUTH TIMING
// ============================================================================
#define BLE_SCAN_DURATION    5
#define DEAUTH_FRAME_DELAY   50
#define BEACON_SPAM_DELAY_MS 100
