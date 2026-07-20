#include "ModuleRegistry.h"

AppModule* ModuleRegistry::modules[MAX_MODULES];
uint8_t ModuleRegistry::count = 0;

void ModuleRegistry::registerModule(AppModule* m) {
    if (count < MAX_MODULES) {
        modules[count++] = m;
    } else {
        Serial.println("[ModuleRegistry] MAX_MODULES atingido, aumente o define.");
    }
}

AppModule* ModuleRegistry::get(uint8_t index) {
    if (index < count) return modules[index];
    return nullptr;
}

uint8_t ModuleRegistry::getCount() {
    return count;
}
