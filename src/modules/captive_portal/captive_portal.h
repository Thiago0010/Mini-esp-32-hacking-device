#pragma once
#include "../../core/AppModule.h"

// ============================================================
//  MODULO: Captive Portal
//  Implemente a logica em captive_portal.cpp
//  Voce so precisa mexer nestes 2 arquivos - o loader
//  (main.cpp) e o ModuleRegistry nao precisam ser alterados.
// ============================================================
class CaptivePortalModule : public AppModule {
public:
    const char* getName() override { return "Captive Portal"; }
    uint16_t getColor() override { return TFT_GOLD; }

    void begin(TFT_eSPI &tft) override;
    void loop(TFT_eSPI &tft) override;
    void onTouch(TFT_eSPI &tft, int16_t x, int16_t y) override;
    void end() override;
};
