#pragma once
#include "../../core/AppModule.h"

// ============================================================
//  MODULO: Evil Portal
//  Implemente a logica em evil_portal.cpp
//  Voce so precisa mexer nestes 2 arquivos - o loader
//  (main.cpp) e o ModuleRegistry nao precisam ser alterados.
// ============================================================
class EvilPortalModule : public AppModule {
public:
    const char* getName() override { return "Evil Portal"; }
    uint16_t getColor() override { return TFT_ORANGE; }

    void begin(TFT_eSPI &tft) override;
    void loop(TFT_eSPI &tft) override;
    void onTouch(TFT_eSPI &tft, int16_t x, int16_t y) override;
    void end() override;
};
