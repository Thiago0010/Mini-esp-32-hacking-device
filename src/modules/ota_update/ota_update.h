#pragma once
#include "../../core/AppModule.h"

// ============================================================
//  MODULO: OTA Update
//  Implemente a logica em ota_update.cpp
//  Voce so precisa mexer nestes 2 arquivos - o loader
//  (main.cpp) e o ModuleRegistry nao precisam ser alterados.
// ============================================================
class OtaUpdateModule : public AppModule {
public:
    const char* getName() override { return "OTA Update"; }
    uint16_t getColor() override { return TFT_SKYBLUE; }

    void begin(TFT_eSPI &tft) override;
    void loop(TFT_eSPI &tft) override;
    void onTouch(TFT_eSPI &tft, int16_t x, int16_t y) override;
    void end() override;
};
