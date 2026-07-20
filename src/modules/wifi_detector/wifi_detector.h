#pragma once
#include "../../core/AppModule.h"

// ============================================================
//  MODULO: Detector WiFi
//  Implemente a logica em wifi_detector.cpp
//  Voce so precisa mexer nestes 2 arquivos - o loader
//  (main.cpp) e o ModuleRegistry nao precisam ser alterados.
// ============================================================
class WifiDetectorModule : public AppModule {
public:
    const char* getName() override { return "Detector WiFi"; }
    uint16_t getColor() override { return TFT_SILVER; }

    void begin(TFT_eSPI &tft) override;
    void loop(TFT_eSPI &tft) override;
    void onTouch(TFT_eSPI &tft, int16_t x, int16_t y) override;
    void end() override;
};
