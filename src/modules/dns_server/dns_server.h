#pragma once
#include "../../core/AppModule.h"

// ============================================================
//  MODULO: DNS Server
//  Implemente a logica em dns_server.cpp
//  Voce so precisa mexer nestes 2 arquivos - o loader
//  (main.cpp) e o ModuleRegistry nao precisam ser alterados.
// ============================================================
class DnsServerModule : public AppModule {
public:
    const char* getName() override { return "DNS Server"; }
    uint16_t getColor() override { return TFT_CYAN; }

    void begin(TFT_eSPI &tft) override;
    void loop(TFT_eSPI &tft) override;
    void onTouch(TFT_eSPI &tft, int16_t x, int16_t y) override;
    void end() override;
};
