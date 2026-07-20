#pragma once
#include "../../core/AppModule.h"

// ============================================================
//  MODULO: Aprender IR
//  Implemente a logica em ir_learn.cpp
//  Voce so precisa mexer nestes 2 arquivos - o loader
//  (main.cpp) e o ModuleRegistry nao precisam ser alterados.
// ============================================================
class IrLearnModule : public AppModule {
public:
    const char* getName() override { return "Aprender IR"; }
    uint16_t getColor() override { return TFT_DARKGREEN; }

    void begin(TFT_eSPI &tft) override;
    void loop(TFT_eSPI &tft) override;
    void onTouch(TFT_eSPI &tft, int16_t x, int16_t y) override;
    void end() override;
};
