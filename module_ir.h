/*
 * ============================================================================
 * [NEV]² - NEV-OS CyberLab v3.0
 * module_ir.h - IR Receiver + IR Replay (TX)
 *
 * APIMELHORADA: Agora com IR TX para retransmitir sinais aprendidos.
 * Requer hardware: LED IR + resistor 100R + transistor 2N2222 no GPIO IR_TX_PIN.
 * ============================================================================
 */
#pragma once

#include <Arduino.h>
#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRsend.h>
#include <IRutils.h>
#include <TFT_eSPI.h>
#include "config.h"
#include "types.h"
#include "ui_helpers.h"

extern TFT_eSPI tft;
extern IRrecv irrecv;
extern IRsend irsend;
extern decode_results irResults;
extern IrState irState;
extern AppState currentState;

inline void initIr() {
  irrecv.enableIRIn();
  irsend.begin();
  strcpy(irState.protocol, "Aguardando...");
  strcpy(irState.value, "---");
  irState.bits = 0;
  irState.count = 0;
  irState.savedValue = 0;
  irState.savedProtocol[0] = '\0';
  irState.savedBits = 0;
  irState.hasSaved = false;
}

inline void drawIrScreen() {
  tft.fillScreen(COLOR_BG);
  drawHeader("IR Receiver", true);

  drawPanel(20, 30, 280, 130);

  tft.setTextColor(COLOR_WARN);
  tft.setTextSize(1);
  tft.setCursor(30, 50);
  tft.print("PROTOCOL:");
  tft.setTextColor(COLOR_TEXT);
  tft.setCursor(110, 50);
  tft.print(irState.protocol);

  tft.setTextColor(COLOR_WARN);
  tft.setCursor(30, 75);
  tft.print("HEX:");
  tft.setTextColor(COLOR_TEXT);
  tft.setCursor(110, 75);
  tft.print(irState.value);

  tft.setTextColor(COLOR_WARN);
  tft.setCursor(30, 100);
  tft.print("BITS:");
  tft.setTextColor(COLOR_TEXT);
  tft.setCursor(110, 100);
  tft.printf("%d", irState.bits);

  tft.setTextColor(COLOR_WARN);
  tft.setCursor(30, 125);
  tft.print("COUNT:");
  tft.setTextColor(COLOR_ACCENT);
  tft.setCursor(110, 125);
  tft.printf("%d sinais", irState.count);

  // Hint
  tft.setTextColor(COLOR_PRIMARY);
  tft.setCursor(10, 175);
  tft.print("Aponte o controle para o receptor");

  tft.setTextColor(COLOR_ACCENT);
  tft.setCursor(10, 195);
  tft.print("Curto: limpa  Longo: salva+replay");
  drawStatusBar("Curto:reset  Longo:salvar  BACK:voltar");
}

inline void enterIrReceive() {
  currentState = STATE_IR;
  drawIrScreen();
}

inline void checkIrSignal() {
  if (irrecv.decode(&irResults)) {
    strncpy(irState.protocol, typeToString(irResults.decode_type).c_str(),
            sizeof(irState.protocol)-1);
    irState.protocol[sizeof(irState.protocol)-1] = '\0';
    strncpy(irState.value, resultToHexidecimal(&irResults).c_str(),
            sizeof(irState.value)-1);
    irState.value[sizeof(irState.value)-1] = '\0';
    irState.bits = irResults.bits;
    irState.count++;
    irrecv.resume();
    if (currentState == STATE_IR) drawIrScreen();
  }
}

inline void drawIrReplayScreen() {
  tft.fillScreen(COLOR_BG);
  drawHeader("IR Replay", true);

  if (!irState.hasSaved) {
    drawInfoBox(
      "Nenhum sinal salvo",
      "Va em IR Receive, receba um sinal\n"
      "e faca toque longo para salvar.\n"
      "Depois volte aqui para retransmitir.",
      COLOR_WARN
    );
    drawStatusBar("Longo:sair  BACK:voltar");
    return;
  }

  drawPanel(20, 30, 280, 130);
  tft.setTextColor(COLOR_WARN);
  tft.setCursor(30, 50);
  tft.print("Salvo:");
  tft.setTextColor(COLOR_TEXT);
  tft.setCursor(90, 50);
  tft.printf("%s", irState.savedProtocol);
  tft.setCursor(30, 75);
  tft.printf("0x%llX", irState.savedValue);
  tft.setCursor(30, 100);
  tft.printf("Bits: %d", irState.savedBits);

  int btnY = 170;
  tft.fillRoundRect(60, btnY, 200, 36, 8, COLOR_PANEL_BG);
  tft.drawRoundRect(60, btnY, 200, 36, 8, COLOR_PRIMARY);
  drawCenteredText("REENVIAR SINAL", btnY + 12, COLOR_PRIMARY, 2);
  drawStatusBar("Longo:transmitir  BACK:voltar");
}

inline void enterIrReplay() {
  currentState = STATE_IR_REPLAY;
  drawIrReplayScreen();
}

inline void replayIr() {
  if (!irState.hasSaved) return;
  Serial.printf("[IR] Replaying %s 0x%llX (%d bits)\n",
                irState.savedProtocol, irState.savedValue, irState.savedBits);
  // Send via IRsend
  irsend.send(irState.savedProtocol, irState.savedValue, irState.savedBits);
  delay(100);
  // Visual feedback
  tft.fillScreen(COLOR_BG);
  drawHeader("IR Replay", true);
  drawCenteredText("SINAL TRANSMITIDO!", 100, COLOR_PRIMARY, 2);
  drawCenteredText("LED IR aceso", 130, COLOR_TEXT, 1);
  delay(800);
  drawIrReplayScreen();
}

// Action: long press in IR screen saves current as "saved"
inline void saveCurrentIr() {
  if (irState.value[0] == '\0' || strcmp(irState.value, "---") == 0) return;
  // Re-decode current to get raw value
  // Note: irResults persists only briefly. We save what we have via hex parsing.
  // For simplicity, we re-read latest if available; otherwise store current.
  irState.savedValue = irResults.value;
  strncpy(irState.savedProtocol, irState.protocol, sizeof(irState.savedProtocol)-1);
  irState.savedProtocol[sizeof(irState.savedProtocol)-1] = '\0';
  irState.savedBits = irResults.bits;
  irState.hasSaved = true;
  Serial.printf("[IR] Saved %s 0x%llX\n", irState.savedProtocol, irState.savedValue);
}
