#pragma once
#include "../../core/AppModule.h"

// ============================================================
//  MODULO: Port Scanner
//  Implemente a logica em port_scanner.cpp
//  Voce so precisa mexer nestes 2 arquivos - o loader
//  (main.cpp) e o ModuleRegistry nao precisam ser alterados.
// ============================================================
class PortScannerModule : public AppModule {
public:
    const char* getName() override { return "Port Scanner"; }
    uint16_t getColor() override { return TFT_YELLOW; }

    void begin(TFT_eSPI &tft) override;
    void loop(TFT_eSPI &tft) override;
    void onTouch(TFT_eSPI &tft, int16_t x, int16_t y) override;
    void end() override;
};
