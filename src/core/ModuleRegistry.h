#pragma once
#include "AppModule.h"

#define MAX_MODULES 32

// ============================================================
//  ModuleRegistry
//  Lista global de todos os módulos registrados no sistema.
//  O loader (main.cpp) só enxerga essa lista — ele não sabe
//  nada sobre DNS Server, BLE Spam, etc. individualmente.
// ============================================================
class ModuleRegistry {
public:
    static void registerModule(AppModule* m);
    static AppModule* get(uint8_t index);
    static uint8_t getCount();

private:
    static AppModule* modules[MAX_MODULES];
    static uint8_t count;
};
