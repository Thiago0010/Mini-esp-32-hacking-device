/*
 * ============================================================================
 * [NEV]² - NEV-OS CyberLab v3.0
 * module_wifi_beacon.h - Beacon spam (random SSIDs)
 * ============================================================================
 */
#pragma once

#include <Arduino.h>
#include <WiFi.h>
#include <TFT_eSPI.h>
#include "config.h"
#include "types.h"
#include "ui_helpers.h"
#include "esp_wifi.h"

extern TFT_eSPI tft;
extern AppState currentState;
extern bool beaconSpamActive;
extern int beaconCount;

// List of funny/random SSIDs to broadcast
const char* const beaconSSIDs[] = {
  "Free WiFi Here", "Starbucks", "FBI Surveillance Van",
  "Pretty Fly for a WiFi", "Virus Detected", "John Connor Network",
  "Get Off My LAN", "Drop It Like Its Hotspot",
  "HillaryClinton", "Yankees Suck", "Get Your Own WiFi",
  "The Promised LAN", "WillUMarryMe", "AbortRetryFail",
  "Loading...", "404 Network Unavailable", "Skynet Global",
  "Area 51", "I am a hacker", "Honk if u love hack",
  "TellMyWiFiLoveHer", "NoMoreRansom", "ItHurtsWhenIP",
  "Titanic Syncing", "Connecting...", "Server is down",
  "Hack the Planet", "Cyberlab_NEVR", "WiFi is Love",
  "Not a trap I promise", "Suspicious WiFi", "Meme Network",
  "I See Dead Packets", "Vampire WiFi", "ZombieNet",
  "HackMeIfYouCan", "Mom's WiFi", "Dad's Internet",
  "Princess Bubblegum", "Finn the Human", "BMO WiFi",
  "Mandela Effect", "False Flag WiFi", "Illuminati Confirmed",
  "CIA Listening Post", "NSA Operation", "KGB WiFi"
};
#define BEACON_SSID_COUNT (sizeof(beaconSSIDs)/sizeof(beaconSSIDs[0]))

// Build a beacon frame for a given SSID
inline size_t buildBeaconFrame(uint8_t* frame, const char* ssid, uint8_t channel) {
  size_t ssidLen = strlen(ssid);
  if (ssidLen > 32) ssidLen = 32;

  // Frame Control: Beacon
  frame[0] = 0x80; frame[1] = 0x00;
  // Duration
  frame[2] = 0x00; frame[3] = 0x00;
  // Destination (broadcast)
  for (int i = 0; i < 6; i++) frame[4+i] = 0xff;
  // Source (random MAC)
  for (int i = 0; i < 6; i++) frame[10+i] = random(0, 256);
  // BSSID (same as source)
  for (int i = 0; i < 6; i++) frame[16+i] = frame[10+i];
  // Sequence control
  frame[22] = 0x00; frame[23] = 0x00;
  // Timestamp (8 bytes)
  for (int i = 0; i < 8; i++) frame[24+i] = 0x00;

  // Beacon interval (100 TU = 102.4ms)
  frame[32] = 0x64; frame[33] = 0x00;
  // Capabilities
  frame[34] = 0x01; frame[35] = 0x04;

  // Tagged parameters:
  // SSID parameter (tag=0, len)
  size_t pos = 36;
  frame[pos++] = 0x00;  // SSID tag
  frame[pos++] = ssidLen;
  memcpy(&frame[pos], ssid, ssidLen);
  pos += ssidLen;

  // Supported rates (tag=1)
  frame[pos++] = 0x01;
  frame[pos++] = 0x08;
  uint8_t rates[] = {0x82, 0x84, 0x8b, 0x96, 0x12, 0x24, 0x48, 0x6c};
  memcpy(&frame[pos], rates, 8);
  pos += 8;

  // Channel (tag=3, len=1)
  frame[pos++] = 0x03;
  frame[pos++] = 0x01;
  frame[pos++] = channel;

  return pos;
}

inline void beaconSpamTask(void* param) {
  uint8_t frame[128];
  int idx = 0;
  uint8_t channels[] = {1, 6, 11};
  int chIdx = 0;

  while (true) {
    if (beaconSpamActive) {
      // Hop channel
      esp_wifi_set_channel(channels[chIdx], WIFI_SECOND_CHAN_NONE);
      chIdx = (chIdx + 1) % 3;

      // Send beacon for current SSID
      size_t len = buildBeaconFrame(frame, beaconSSIDs[idx], channels[chIdx]);
      esp_wifi_80211_tx(WIFI_IF_STA, frame, len, false);
      beaconCount++;
      idx = (idx + 1) % BEACON_SSID_COUNT;
      delay(BEACON_SPAM_DELAY_MS);
    } else {
      delay(50);
    }
  }
}

inline void drawBeaconScreen() {
  tft.fillScreen(COLOR_BG);
  drawHeader("Beacon Spam", true);
  drawPanel(20, 30, 280, 50);
  tft.setTextColor(COLOR_TEXT);
  tft.setCursor(30, 42);
  tft.print("Status:");
  tft.setTextColor(beaconSpamActive ? COLOR_DANGER : COLOR_PRIMARY);
  tft.setCursor(80, 42);
  tft.print(beaconSpamActive ? "ATIVO" : "IDLE");

  drawPanel(20, 90, 280, 50);
  tft.setTextColor(COLOR_TEXT);
  tft.setCursor(30, 102);
  tft.print("Beacons:");
  tft.setTextColor(COLOR_WARN);
  tft.setCursor(90, 102);
  tft.printf("%d", beaconCount);

  // Action button
  int btnY = 160;
  tft.fillRoundRect(60, btnY, 200, 40, 8, beaconSpamActive ? 0x4208 : COLOR_PANEL_BG);
  tft.drawRoundRect(60, btnY, 200, 40, 8, beaconSpamActive ? COLOR_DANGER : COLOR_PRIMARY);
  drawCenteredText(beaconSpamActive ? "PARAR" : "INICIAR SPAM", btnY + 14,
                   beaconSpamActive ? COLOR_DANGER : COLOR_PRIMARY, 2);
  drawStatusBar("Longo:toggle  BACK:sair");
}

inline void enterWifiBeacon() {
  currentState = STATE_WIFI_BEACON;
  beaconSpamActive = false;
  beaconCount = 0;
  drawBeaconScreen();
}
