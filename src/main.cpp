#include <Arduino.h>
#include <SPI.h>
#include <TFT_eSPI.h>
#include <XPT2046_Touchscreen.h>

#include "core/AppModule.h"
#include "core/ModuleRegistry.h"
#include "core/UI.h"
#include "modules_list.h"

// ============================================================
//  CONFIG DE HARDWARE
//  Pinos abaixo são os "padrão" da placa CYD
//  (ESP32-2432S028R, display ILI9341 + touch resistivo XPT2046).
//  Se sua placa/tela for outra, ajuste aqui e no User_Setup
//  da lib TFT_eSPI (veja README.md).
// ============================================================
#define XPT2046_IRQ  36
#define XPT2046_MOSI 32
#define XPT2046_MISO 39
#define XPT2046_CLK  25
#define XPT2046_CS   33

TFT_eSPI tft = TFT_eSPI();
SPIClass touchSPI = SPIClass(VSPI);
XPT2046_Touchscreen ts(XPT2046_CS, XPT2046_IRQ);

enum SystemState { STATE_MENU, STATE_APP };
SystemState state = STATE_MENU;
int8_t activeModuleIndex = -1;

// Calibração simples do touch resistivo.
// Se o toque estiver "errado" na sua tela, ajuste esses 4 números
// (min/max de X e Y crus lidos do XPT2046).
int16_t mapTouchX(int16_t raw) { return constrain(map(raw, 200, 3800, 0, tft.width()), 0, tft.width()); }
int16_t mapTouchY(int16_t raw) { return constrain(map(raw, 240, 3800, 0, tft.height()), 0, tft.height()); }

void goToMenu() {
    if (activeModuleIndex >= 0) {
        AppModule* m = ModuleRegistry::get(activeModuleIndex);
        if (m) m->end();
    }
    activeModuleIndex = -1;
    state = STATE_MENU;
    UI::drawMenu(tft);
}

void openModule(int8_t index) {
    AppModule* m = ModuleRegistry::get(index);
    if (!m) return;

    activeModuleIndex = index;
    state = STATE_APP;
    tft.fillScreen(TFT_BLACK);
    m->begin(tft);
    UI::drawBackButton(tft);
}

void setup() {
    Serial.begin(115200);

    tft.init();
    tft.setRotation(1);
    tft.fillScreen(TFT_BLACK);

    touchSPI.begin(XPT2046_CLK, XPT2046_MISO, XPT2046_MOSI, XPT2046_CS);
    ts.begin(touchSPI);
    ts.setRotation(1);

    // >>> Aqui é onde todos os módulos entram em cena. <<<
    // Essa função vive em src/modules_list.cpp — é o único
    // lugar que precisa de uma linha nova quando você criar
    // um módulo, main.cpp fica intocado.
    registerAllModules();

    Serial.printf("[loader] %d modulos registrados\n", ModuleRegistry::getCount());

    UI::drawMenu(tft);
}

void loop() {
    if (ts.touched()) {
        TS_Point p = ts.getPoint();
        int16_t x = mapTouchX(p.x);
        int16_t y = mapTouchY(p.y);

        if (state == STATE_MENU) {
            int8_t idx = UI::hitTestMenu(tft, x, y);
            if (idx >= 0) openModule(idx);

        } else { // STATE_APP
            if (UI::hitBackButton(tft, x, y)) {
                goToMenu();
            } else {
                AppModule* m = ModuleRegistry::get(activeModuleIndex);
                if (m) m->onTouch(tft, x, y);
            }
        }

        delay(150); // debounce simples de toque
    }

    if (state == STATE_APP && activeModuleIndex >= 0) {
        AppModule* m = ModuleRegistry::get(activeModuleIndex);
        if (m) m->loop(tft);
    }
}
