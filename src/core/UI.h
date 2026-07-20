#pragma once
#include <TFT_eSPI.h>
#include "AppModule.h"

// ============================================================
//  UI
//  Desenha a tela inicial (grade de ícones, no mesmo espírito
//  "toque no ícone pra abrir o app") e o botão de voltar usado
//  dentro de cada módulo. Isso é 100% genérico: não sabe nada
//  sobre as funcionalidades específicas.
// ============================================================
namespace UI {

    // Quantidade de colunas na grade do menu
    const uint8_t GRID_COLS = 3;
    const uint8_t ICON_MARGIN = 8;
    const uint8_t ICON_H = 60;
    const uint8_t BACK_BTN_H = 28;

    // Desenha a tela de menu completa (todos os ícones registrados)
    void drawMenu(TFT_eSPI &tft);

    // Dado um toque (x,y) na tela de menu, retorna o índice do
    // módulo tocado, ou -1 se não tocou em nenhum ícone.
    int8_t hitTestMenu(TFT_eSPI &tft, int16_t x, int16_t y);

    // Desenha a barra superior com botão "< Voltar" dentro de um app
    void drawBackButton(TFT_eSPI &tft);

    // Testa se o toque (x,y) caiu no botão de voltar
    bool hitBackButton(TFT_eSPI &tft, int16_t x, int16_t y);
}
