#pragma once
#include "../../core/AppModule.h"

// ============================================================
//  MODULO: BLE Keyboard
//  Implemente a logica em ble_keyboard.cpp
//  Voce so precisa mexer nestes 2 arquivos - o loader
//  (main.cpp) e o ModuleRegistry nao precisam ser alterados.
// ============================================================
class BleKeyboardModule : public AppModule {
public:
    const char* getName() override { return "BLE Keyboard"; }
    uint16_t getColor() override { return TFT_MAGENTA; }

    void begin(TFT_eSPI &tft) override;
    void loop(TFT_eSPI &tft) override;
    void onTouch(TFT_eSPI &tft, int16_t x, int16_t y) override;
    void end() override;
};
