#pragma once
#include "../../core/AppModule.h"

// ============================================================
//  MODULO: WiFi Scanner
//  Implemente a logica em wifi_scanner.cpp
//  Voce so precisa mexer nestes 2 arquivos - o loader
//  (main.cpp) e o ModuleRegistry nao precisam ser alterados.
// ============================================================
class WifiScannerModule : public AppModule {
public:
    const char* getName() override { return "WiFi Scanner"; }
    uint16_t getColor() override { return TFT_GREEN; }

    void begin(TFT_eSPI &tft) override;
    void loop(TFT_eSPI &tft) override;
    void onTouch(TFT_eSPI &tft, int16_t x, int16_t y) override;
    void end() override;
};
