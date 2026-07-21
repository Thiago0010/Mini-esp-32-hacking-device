/*
 * ============================================================================
 * [NEV]² - NEV-OS CyberLab v3.0
 * mainhck.ino - Setup + loop principal
 *
 * ESTRUTURA: Este arquivo contem APENAS:
 *   - Includes
 *   - Variaveis globais
 *   - setup()
 *   - loop()
 *   - dispatchers de acao (curto/longo) por estado
 *
 * Toda a logica de feature esta nos headers locais:
 *   config.h, types.h, assets.h, touch_driver.h, ui_helpers.h,
 *   menu_system.h, e os module_*.h
 *
 * ATENCAO: O touch bit-bang (lerTouchTotalmenteIsolado) eh SENSIVEL e
 * NAO foi alterado. Esta em touch_driver.h e foi preservado exatamente
 * como no v2.0.
 *
 * ADVERTENCIA LEGAL: Use este projeto apenas em redes e equipamentos
 * proprios ou com autorizacao explicita. Pentesting sem autorizacao
 * eh crime (Art. 154-A CP Brasil, equivalentes em outros paises).
 * ============================================================================
 */

// ============================================================================
// THIRD PARTY LIBRARIES
// ============================================================================
#include <TFT_eSPI.h>
#include <Adafruit_GFX.h>
#include <Fonts/Picopixel.h>
#include <SPI.h>
#include <FS.h>
#include <SD.h>
#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>
#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRsend.h>
#include <IRutils.h>
#include <ArduinoOTA.h>
#include <NimBLEDevice.h>
#include <NimBLEScan.h>
#include <NimBLEAdvertisedDevice.h>
#include <NimBLEAdvertising.h>
#include <ESP32Ping.h>
#include "esp_wifi.h"
#include "esp_now.h"

// ============================================================================
// LOCAL HEADERS (conteudo concatenado pela Arduino IDE)
// ============================================================================
#include "config.h"
#include "types.h"
#include "assets.h"
#include "touch_driver.h"
#include "ui_helpers.h"
#include "menu_system.h"
#include "module_ir.h"
#include "module_sd.h"
#include "module_game.h"
#include "module_wifi_scan.h"
#include "module_wifi_deauth.h"
#include "module_wifi_captive.h"
#include "module_wifi_evil.h"
#include "module_wifi_beacon.h"
#include "module_wifi_detector.h"
#include "module_wifi_ping.h"
#include "module_wifi_portscan.h"
#include "module_wifi_ftp.h"
#include "module_wifi_ota.h"
#include "module_ble_scan.h"
#include "module_ble_spam.h"
#include "module_ble_adv.h"
#include "module_ble_sniff.h"
#include "module_ble_attack.h"
#include "module_ble_mouse.h"
#include "module_ble_kbd.h"
#include "module_badusb.h"

// ============================================================================
// GLOBALS
// ============================================================================
TFT_eSPI tft = TFT_eSPI();
IRrecv irrecv(IR_RX_PIN);
IRsend irsend(IR_TX_PIN);
decode_results irResults;
DNSServer dnsServer;
WebServer webServer(80);

AppState currentState = STATE_MENU;
MenuNav nav;
TouchState touchState;
PongState pong;
IrState irState;

// Wi-Fi scan globals
Network _networks[MAX_NETWORKS];
int wifiNetworksFound = -1;
int wifiAnalyserPage = 0;

// SD globals
bool sdCardReady = false;
unsigned long sdCardSizeMB = 0;
unsigned long sdUsedMB = 0;
char sdFileList[MAX_SD_FILES][SD_FILE_NAME_LEN];
int sdFileCount = 0;
int sdPage = 0;

// Deauth globals
bool isAttacking = false;
uint8_t targetAP[6] = {0};
uint8_t targetClient[6] = {0};

// Beacon spam globals
bool beaconSpamActive = false;
int beaconCount = 0;

// Wi-Fi detector globals
bool detectorActive = false;
int detectorLogCount = 0;
char detectorLog[20][40];

// Ping globals
bool pingActive = false;
int pingCurrent = 0;
int pingFound = 0;
int pingTotal = 254;

// Port scan globals
PortResult portResults[16];
int portScanCurrent = 0;
int portScanTotal = 0;
bool portScanActive = false;
char portScanTarget[20] = "0.0.0.0";

// FTP globals
char ftpHost[32] = "192.168.4.1";
char ftpUser[16] = "anonymous";
char ftpPass[16] = "anon@";
int ftpPort = 21;
char ftpLastResponse[64] = "(nao conectado)";

// OTA globals
bool otaInProgress = false;
int otaProgress = 0;

// Evil portal globals
int evilTemplateIdx = 0;

// BLE globals
BleDevice bleDevices[MAX_BLE_DEVICES];
int bleDeviceCount = 0;
bool bleScanActive = false;
NimBLEScan* pBleScan = nullptr;

bool bleSpamActive = false;
int bleSpamCount = 0;

bool bleAdvActive = false;
char bleAdvName[20] = "NEV-OS";

bool bleSniffActive = false;
int bleSniffCount = 0;
char bleSniffLog[12][40];

bool bleAttackActive = false;
int bleAttackCount = 0;

// ============================================================================
// FORWARD DECLARATIONS for menu items
// ============================================================================
void enterWifiMenu() { nav.stack[nav.stackDepth] = wifiChildren; nav.selectedIndex[nav.stackDepth] = 0; strcpy(nav.currentTitle, "Wi-Fi Tools"); drawMenuList(); }
void enterBleMenu()  { nav.stack[nav.stackDepth] = bleChildren;  nav.selectedIndex[nav.stackDepth] = 0; strcpy(nav.currentTitle, "BLE Tools");  drawMenuList(); }
void enterMiscMenu() { nav.stack[nav.stackDepth] = miscChildren; nav.selectedIndex[nav.stackDepth] = 0; strcpy(nav.currentTitle, "Misc & Demo"); drawMenuList(); }
void enterSettingsMenu() { nav.stack[nav.stackDepth] = settingsChildren; nav.selectedIndex[nav.stackDepth] = 0; strcpy(nav.currentTitle, "Settings"); drawMenuList(); }

// ============================================================================
// ACTION DISPATCHER - called by handleTouch based on current state
// ============================================================================

void onShortPress() {
  switch (currentState) {
    case STATE_MENU:
      menuNext();
      drawMenuList();
      break;
    case STATE_WIFI_SCAN:
      handleWifiScanShort();
      break;
    case STATE_WIFI_DEAUTH:
      handleDeauthShort();
      break;
    default:
      // Most sub-menus use long-press for action
      // Clear IR display on short press
      if (currentState == STATE_IR) {
        strcpy(irState.protocol, "Aguardando...");
        strcpy(irState.value, "---");
        irState.bits = 0;
        drawIrScreen();
      }
      break;
  }
}

void onLongPress() {
  // If touching BACK button, navigate back
  if (touchState.lastX >= BACK_BTN_X && touchState.lastX <= BACK_BTN_X + BACK_BTN_W &&
      touchState.lastY >= BACK_BTN_Y && touchState.lastY <= BACK_BTN_Y + BACK_BTN_H) {
    // Stop active things based on current state
    switch (currentState) {
      case STATE_WIFI_CAPTIVE: exitWifiCaptive(); break;
      case STATE_WIFI_EVIL:    exitWifiEvil(); break;
      default: break;
    }
    isAttacking = false;
    beaconSpamActive = false;
    detectorActive = false;
    if (bleScanActive) { if (pBleScan) pBleScan->stop(); bleScanActive = false; }
    if (bleSpamActive) bleSpamActive = false;
    if (bleAdvActive) toggleBleAdv();
    if (bleSniffActive) toggleBleSniff();
    if (bleAttackActive) toggleBleAttack();
    pingActive = false;
    portScanActive = false;
    currentState = STATE_MENU;
    drawMenuList();
    return;
  }

  switch (currentState) {
    case STATE_MENU:
      menuEnter();
      // If menuEnter went into a submenu, redraw; if leaf, function already called
      if (currentState == STATE_MENU) {
        // re-render
        drawMenuList();
      }
      break;
    case STATE_WIFI_SCAN:
      // re-scan
      enterWifiScan();
      break;
    case STATE_WIFI_DEAUTH:
      handleDeauthLong();
      break;
    case STATE_WIFI_BEACON:
      beaconSpamActive = !beaconSpamActive;
      drawBeaconScreen();
      break;
    case STATE_WIFI_DETECTOR:
      toggleWifiDetector();
      break;
    case STATE_WIFI_PING:
      if (!pingActive) startPingScan();
      break;
    case STATE_WIFI_PORTSCAN:
      if (!portScanActive) startPortScan();
      break;
    case STATE_WIFI_FTP:
      testFtpConnection();
      break;
    case STATE_BLE_SCAN:
      startBleScan();
      break;
    case STATE_BLE_SPAM:
      bleSpamActive = !bleSpamActive;
      if (bleSpamActive && !NimBLEDevice::isInitialized()) {
        NimBLEDevice::init("");
      }
      drawBleSpamScreen();
      break;
    case STATE_BLE_ADV:
      toggleBleAdv();
      break;
    case STATE_BLE_SNIFF:
      toggleBleSniff();
      break;
    case STATE_BLE_ATTACK:
      toggleBleAttack();
      break;
    case STATE_IR:
      saveCurrentIr();
      drawIrScreen();
      break;
    case STATE_IR_REPLAY:
      replayIr();
      break;
    case STATE_SD:
      updateSdInfo();
      drawSdScreen();
      break;
    case STATE_BADUSB:
    case STATE_BLE_MOUSE:
    case STATE_BLE_KBD:
      // Just stay (no action)
      break;
    case STATE_GAME:
      // touch is direct in updatePongGame
      break;
    default: break;
  }
}

// ============================================================================
// SETUP
// ============================================================================
void setup() {
  Serial.begin(115200);
  pinMode(IR_RX_PIN, INPUT);
  pinMode(IR_TX_PIN, OUTPUT); digitalWrite(IR_TX_PIN, LOW);
  pinMode(TFT_CS, OUTPUT); digitalWrite(TFT_CS, HIGH);
  pinMode(TOUCH_CS, OUTPUT); digitalWrite(TOUCH_CS, HIGH);
  pinMode(TOUCH_CLK, OUTPUT);
  pinMode(TOUCH_MOSI, OUTPUT);
  pinMode(TOUCH_MISO, INPUT);

  tft.init();
  tft.setRotation(1);
  tft.fillScreen(COLOR_BG);

  initIr();
  initTouch();
  initSd();
  menuInit();

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  // Background tasks
  xTaskCreate(attackTask, "Attack", 2048, NULL, 1, NULL);
  xTaskCreate(beaconSpamTask, "Beacon", 2048, NULL, 1, NULL);

  showBootIntro();
  drawMenuList();
}

// ============================================================================
// LOOP
// ============================================================================
void loop() {
  // Game runs its own tight loop
  if (currentState == STATE_GAME) {
    updatePongGame();
    delay(20);
    return;
  }

  // IR listening (always)
  checkIrSignal();

  // OTA always available
  if (currentState == STATE_WIFI_OTA) {
    handleOtaLoop();
  }

  // Active attacks loops
  if (bleSpamActive) bleSpamLoop();
  if (bleAttackActive) bleAttackLoop();
  if (currentState == STATE_WIFI_DETECTOR && detectorActive) {
    drawDetectorScreen();
  }

  // Captive / Evil portal handle
  if (currentState == STATE_WIFI_CAPTIVE) handleCaptiveLoop();
  if (currentState == STATE_WIFI_EVIL) handleEvilLoop();

  // Touch handler
  handleTouchGeneric(onShortPress, onLongPress);
}
