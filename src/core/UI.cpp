#include "UI.h"
#include "ModuleRegistry.h"

namespace UI {

// calcula a posição (x,y,w,h) do ícone "index" na grade
static void iconRect(TFT_eSPI &tft, uint8_t index, int16_t &x, int16_t &y, int16_t &w, int16_t &h) {
    uint8_t col = index % GRID_COLS;
    uint8_t row = index / GRID_COLS;

    w = (tft.width() - ICON_MARGIN * (GRID_COLS + 1)) / GRID_COLS;
    h = ICON_H;
    x = ICON_MARGIN + col * (w + ICON_MARGIN);
    y = ICON_MARGIN + row * (h + ICON_MARGIN);
}

void drawMenu(TFT_eSPI &tft) {
    tft.fillScreen(TFT_BLACK);

    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setTextSize(1);
    tft.setCursor(ICON_MARGIN, 2);
    tft.print("MINI LAB - selecione um modulo");

    uint8_t total = ModuleRegistry::getCount();
    for (uint8_t i = 0; i < total; i++) {
        AppModule* m = ModuleRegistry::get(i);
        if (!m) continue;

        int16_t x, y, w, h;
        iconRect(tft, i, x, y, w, h);
        // desloca tudo pra baixo pra abrir espaço do título
        y += 14;

        tft.fillRoundRect(x, y, w, h, 6, m->getColor());
        tft.drawRoundRect(x, y, w, h, 6, TFT_WHITE);

        tft.setTextColor(TFT_BLACK, m->getColor());
        tft.setTextSize(1);
        // centraliza o texto de forma simples, quebrando se precisar
        int16_t textX = x + 4;
        int16_t textY = y + h / 2 - 4;
        tft.setCursor(textX, textY);
        tft.print(m->getName());
    }
}

int8_t hitTestMenu(TFT_eSPI &tft, int16_t x, int16_t y) {
    uint8_t total = ModuleRegistry::getCount();
    for (uint8_t i = 0; i < total; i++) {
        int16_t ix, iy, iw, ih;
        iconRect(tft, i, ix, iy, iw, ih);
        iy += 14;

        if (x >= ix && x <= ix + iw && y >= iy && y <= iy + ih) {
            return i;
        }
    }
    return -1;
}

void drawBackButton(TFT_eSPI &tft) {
    tft.fillRect(0, tft.height() - BACK_BTN_H, tft.width(), BACK_BTN_H, TFT_RED);
    tft.setTextColor(TFT_WHITE, TFT_RED);
    tft.setTextSize(1);
    tft.setCursor(8, tft.height() - BACK_BTN_H + 9);
    tft.print("< VOLTAR AO MENU");
}

bool hitBackButton(TFT_eSPI &tft, int16_t x, int16_t y) {
    return y >= (tft.height() - BACK_BTN_H);
}

} // namespace UI
