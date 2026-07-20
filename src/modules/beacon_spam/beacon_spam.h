#pragma once
#include "../../core/AppModule.h"

// ============================================================
//  MODULO: Beacon Spam
//  Implemente a logica em beacon_spam.cpp
//  Voce so precisa mexer nestes 2 arquivos - o loader
//  (main.cpp) e o ModuleRegistry nao precisam ser alterados.
// ============================================================
class BeaconSpamModule : public AppModule {
public:
    const char* getName() override { return "Beacon Spam"; }
    uint16_t getColor() override { return TFT_RED; }

    void begin(TFT_eSPI &tft) override;
    void loop(TFT_eSPI &tft) override;
    void onTouch(TFT_eSPI &tft, int16_t x, int16_t y) override;
    void end() override;
};
