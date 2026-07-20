#include "wifi_detector.h"

// ============================================================
//  MODULO: Detector WiFi
//  Escreva aqui a funcionalidade. As 4 funcoes abaixo sao o
//  unico contrato que o loader espera - implemente o que
//  precisar dentro delas.
// ============================================================

void WifiDetectorModule::begin(TFT_eSPI &tft) {
    // Chamado 1x quando o usuario abre o app.
    // Desenhe a tela inicial e inicialize o que for preciso aqui.
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setCursor(10, 10);
    tft.println("Detector WiFi");
    tft.setCursor(10, 30);
    tft.println("TODO: implemente aqui");
}

void WifiDetectorModule::loop(TFT_eSPI &tft) {
    // Chamado continuamente enquanto o app estiver aberto.
    // TODO
}

void WifiDetectorModule::onTouch(TFT_eSPI &tft, int16_t x, int16_t y) {
    // Chamado quando o usuario toca na tela dentro do app.
    // TODO
}

void WifiDetectorModule::end() {
    // Chamado 1x quando o usuario aperta "voltar".
    // Pare servidores, desligue radio, libere memoria, etc.
    // TODO
}
