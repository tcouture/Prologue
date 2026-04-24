#include "UIWidgets.h"
#include <string.h>

namespace UIWidgets {

void drawKnob(ILI9341_t3& g, int x, int y, int w, int h,
              const char* label, const char* valueStr,
              float value01, bool selected) {
    uint16_t panel = selected ? Theme::PANEL_SEL : Theme::PANEL;
    uint16_t arcCol = selected ? Theme::KNOB_SEL : Theme::KNOB_FILL;

    g.fillRoundRect(x, y, w, h, 4, panel);
    g.drawRoundRect(x, y, w, h, 4, selected ? Theme::ACCENT_AMBER : Theme::GRID);

    g.setTextColor(Theme::TEXT_DIM);
    g.setTextSize(1);
    g.setCursor(x + 4, y + 4);
    g.print(label);

    int barY = y + 20;
    int barH = 6;
    int barW = w - 8;
    int barX = x + 4;
    g.fillRect(barX, barY, barW, barH, Theme::KNOB_TRACK);
    int fillW = (int)(barW * value01);
    if (fillW > 0) g.fillRect(barX, barY, fillW, barH, arcCol);

    g.setTextColor(Theme::TEXT);
    g.setTextSize(2);
    g.setCursor(x + 6, y + 36);
    g.print(valueStr);
}

void drawTab(ILI9341_t3& g, int x, int y, int w, int h,
             const char* text, bool active) {
    uint16_t bg = active ? Theme::TAB_ACTIVE : Theme::TAB_IDLE;
    uint16_t fg = active ? Theme::BG : Theme::TEXT;
    g.fillRect(x, y, w, h, bg);
    g.setTextColor(fg);
    g.setTextSize(1);
    int tx = x + (w - (int)strlen(text) * 6) / 2;
    int ty = y + (h - 8) / 2;
    g.setCursor(tx, ty);
    g.print(text);
}

void drawButton(ILI9341_t3& g, int x, int y, int w, int h,
                const char* text, bool pressed) {
    uint16_t bg = pressed ? Theme::ACCENT_AMBER : Theme::PANEL;
    uint16_t fg = pressed ? Theme::BG : Theme::TEXT;
    g.fillRoundRect(x, y, w, h, 4, bg);
    g.drawRoundRect(x, y, w, h, 4, Theme::GRID);
    g.setTextColor(fg);
    g.setTextSize(1);
    int tx = x + (w - (int)strlen(text) * 6) / 2;
    int ty = y + (h - 8) / 2;
    g.setCursor(tx, ty);
    g.print(text);
}

}  // namespace UIWidgets
