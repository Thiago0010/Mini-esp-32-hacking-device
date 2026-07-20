#pragma once
#include "../../core/AppModule.h"

// ============================================================
//  MODULO: BLE Mouse
//  Implemente a logica em ble_mouse.cpp
//  Voce so precisa mexer nestes 2 arquivos - o loader
//  (main.cpp) e o ModuleRegistry nao precisam ser alterados.
// ============================================================
class BleMouseModule : public AppModule {
public:
    const char* getName() override { return "BLE Mouse"; }
    uint16_t getColor() override { return TFT_PINK; }

    void begin(TFT_eSPI &tft) override;
    void loop(TFT_eSPI &tft) override;
    void onTouch(TFT_eSPI &tft, int16_t x, int16_t y) override;
    void end() override;
};
