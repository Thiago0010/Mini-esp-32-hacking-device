/*
 * ============================================================================
 * [NEV]² - NEV-OS CyberLab v3.0
 * module_wifi_ftp.h - Basic FTP client
 * ============================================================================
 */
#pragma once

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <TFT_eSPI.h>
#include "config.h"
#include "types.h"
#include "ui_helpers.h"

extern TFT_eSPI tft;
extern AppState currentState;
extern char ftpHost[32];
extern char ftpUser[16];
extern char ftpPass[16];
extern int ftpPort;
extern char ftpLastResponse[64];

inline void sendFtpCommand(WiFiClient& client, const char* cmd, const char* arg = nullptr) {
  if (arg) client.printf("%s %s\r\n", cmd, arg);
  else client.printf("%s\r\n", cmd);
  delay(100);
  String resp = client.readStringUntil('\n');
  strncpy(ftpLastResponse, resp.c_str(), sizeof(ftpLastResponse)-1);
  ftpLastResponse[sizeof(ftpLastResponse)-1] = '\0';
  resp.trim();
  int code = resp.substring(0, 3).toInt();
  Serial.printf("[FTP] -> %d %s\n", code, resp.c_str());
}

inline void drawFtpScreen() {
  tft.fillScreen(COLOR_BG);
  drawHeader("FTP Client", true);

  if (WiFi.status() != WL_CONNECTED) {
    drawPanel(20, 40, 280, 60);
    tft.setTextColor(COLOR_WARN);
    tft.setCursor(30, 60);
    tft.print("Conecte ao Wi-Fi primeiro");
    drawStatusBar("Longo:sair  BACK:voltar");
    return;
  }

  drawPanel(20, 30, 280, 110);
  tft.setTextColor(COLOR_TEXT);
  tft.setTextSize(1);
  tft.setCursor(30, 42);
  tft.print("Host:");
  tft.setTextColor(COLOR_WARN);
  tft.setCursor(70, 42);
  tft.printf("%s:%d", ftpHost, ftpPort);
  tft.setTextColor(COLOR_TEXT);
  tft.setCursor(30, 60);
  tft.print("User:");
  tft.setTextColor(COLOR_ACCENT);
  tft.setCursor(70, 60);
  tft.print(ftpUser);
  tft.setTextColor(COLOR_TEXT);
  tft.setCursor(30, 78);
  tft.print("Pass:");
  tft.setTextColor(COLOR_ACCENT);
  tft.setCursor(70, 78);
  tft.print("*****");
  tft.setTextColor(COLOR_TEXT);
  tft.setCursor(30, 100);
  tft.print("Status:");
  tft.setTextColor(COLOR_PRIMARY);
  tft.setCursor(80, 100);
  tft.print(ftpLastResponse);

  // Action hint
  drawPanel(20, 150, 280, 60);
  drawCenteredText("Longo: conectar e LIST", 175, COLOR_TEXT, 1);
  drawCenteredText("BACK: sair", 195, COLOR_TEXT, 1);

  drawStatusBar("Longo:testar conexao  BACK:voltar");
}

inline void enterWifiFtp() {
  currentState = STATE_WIFI_FTP;
  strncpy(ftpHost, "192.168.4.1", sizeof(ftpHost)-1);
  strncpy(ftpUser, "anonymous", sizeof(ftpUser)-1);
  strncpy(ftpPass, "anon@", sizeof(ftpPass)-1);
  ftpPort = 21;
  strcpy(ftpLastResponse, "(nao conectado)");
  drawFtpScreen();
}

inline void testFtpConnection() {
  if (WiFi.status() != WL_CONNECTED) return;
  WiFiClient client;
  tft.fillScreen(COLOR_BG);
  drawHeader("FTP Client - Conectando", true);
  drawCenteredText("Conectando...", 110, COLOR_WARN, 1);
  drawStatusBar("Aguarde...");

  if (!client.connect(ftpHost, ftpPort, 3000)) {
    strcpy(ftpLastResponse, "FALHA ao conectar");
    drawFtpScreen();
    return;
  }

  delay(500);
  String welcome = client.readStringUntil('\n');
  strncpy(ftpLastResponse, welcome.c_str(), sizeof(ftpLastResponse)-1);
  ftpLastResponse[sizeof(ftpLastResponse)-1] = '\0';

  // Login
  client.printf("USER %s\r\n", ftpUser);
  delay(200);
  String u = client.readStringUntil('\n');
  client.printf("PASS %s\r\n", ftpPass);
  delay(200);
  String p = client.readStringUntil('\n');
  snprintf(ftpLastResponse, sizeof(ftpLastResponse), "Login: %s", p.c_str());

  // SYST
  client.printf("SYST\r\n");
  delay(200);
  String s = client.readStringUntil('\n');
  client.printf("QUIT\r\n");
  delay(100);
  client.stop();
  drawFtpScreen();
}
