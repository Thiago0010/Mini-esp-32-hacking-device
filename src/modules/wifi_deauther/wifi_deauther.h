#pragma once
#include "../../core/AppModule.h"

// ============================================================
//  MODULO: WiFi Deauther
//  Implemente a logica em wifi_deauther.cpp
//  Voce so precisa mexer nestes 2 arquivos - o loader
//  (main.cpp) e o ModuleRegistry nao precisam ser alterados.
// ============================================================
class WifiDeautherModule : public AppModule {
public:
    const char* getName() override { return "WiFi Deauther"; }
    uint16_t getColor() override { return TFT_RED; }

    void begin(TFT_eSPI &tft) override;
    void loop(TFT_eSPI &tft) override;
    void onTouch(TFT_eSPI &tft, int16_t x, int16_t y) override;
    void end() override;
};
