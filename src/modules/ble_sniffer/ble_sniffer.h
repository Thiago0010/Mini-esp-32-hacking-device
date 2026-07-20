#pragma once
#include "../../core/AppModule.h"

// ============================================================
//  MODULO: BLE Sniffer
//  Implemente a logica em ble_sniffer.cpp
//  Voce so precisa mexer nestes 2 arquivos - o loader
//  (main.cpp) e o ModuleRegistry nao precisam ser alterados.
// ============================================================
class BleSnifferModule : public AppModule {
public:
    const char* getName() override { return "BLE Sniffer"; }
    uint16_t getColor() override { return TFT_PURPLE; }

    void begin(TFT_eSPI &tft) override;
    void loop(TFT_eSPI &tft) override;
    void onTouch(TFT_eSPI &tft, int16_t x, int16_t y) override;
    void end() override;
};
