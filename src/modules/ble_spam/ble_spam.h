#pragma once
#include "../../core/AppModule.h"

// ============================================================
//  MODULO: BLE Spam
//  Implemente a logica em ble_spam.cpp
//  Voce so precisa mexer nestes 2 arquivos - o loader
//  (main.cpp) e o ModuleRegistry nao precisam ser alterados.
// ============================================================
class BleSpamModule : public AppModule {
public:
    const char* getName() override { return "BLE Spam"; }
    uint16_t getColor() override { return TFT_RED; }

    void begin(TFT_eSPI &tft) override;
    void loop(TFT_eSPI &tft) override;
    void onTouch(TFT_eSPI &tft, int16_t x, int16_t y) override;
    void end() override;
};
