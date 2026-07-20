#include "ftp_client.h"

// ============================================================
//  MODULO: Cliente FTP
//  Escreva aqui a funcionalidade. As 4 funcoes abaixo sao o
//  unico contrato que o loader espera - implemente o que
//  precisar dentro delas.
// ============================================================

void FtpClientModule::begin(TFT_eSPI &tft) {
    // Chamado 1x quando o usuario abre o app.
    // Desenhe a tela inicial e inicialize o que for preciso aqui.
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setCursor(10, 10);
    tft.println("Cliente FTP");
    tft.setCursor(10, 30);
    tft.println("TODO: implemente aqui");
}

void FtpClientModule::loop(TFT_eSPI &tft) {
    // Chamado continuamente enquanto o app estiver aberto.
    // TODO
}

void FtpClientModule::onTouch(TFT_eSPI &tft, int16_t x, int16_t y) {
    // Chamado quando o usuario toca na tela dentro do app.
    // TODO
}

void FtpClientModule::end() {
    // Chamado 1x quando o usuario aperta "voltar".
    // Pare servidores, desligue radio, libere memoria, etc.
    // TODO
}
