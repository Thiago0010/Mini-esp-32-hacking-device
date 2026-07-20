#pragma once
#include "../../core/AppModule.h"

// ============================================================
//  MODULO: Bad USB
//  Implemente a logica em bad_usb.cpp
//  Voce so precisa mexer nestes 2 arquivos - o loader
//  (main.cpp) e o ModuleRegistry nao precisam ser alterados.
// ============================================================
class BadUsbModule : public AppModule {
public:
    const char* getName() override { return "Bad USB"; }
    uint16_t getColor() override { return TFT_MAROON; }

    void begin(TFT_eSPI &tft) override;
    void loop(TFT_eSPI &tft) override;
    void onTouch(TFT_eSPI &tft, int16_t x, int16_t y) override;
    void end() override;
};
