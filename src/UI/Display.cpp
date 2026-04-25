#include "Display.h"

void Display::begin() {
    _tft.begin();
    _tft.setRotation(3);
    _tft.fillScreen(Theme::BG);
    _tft.setTextColor(Theme::TEXT);
    _tft.setTextSize(1);

    _touch.begin();
    _touch.setRotation(1);
    pinMode(TOUCH_IRQ, INPUT);
}

bool Display::touched() {
    return _touch.touched();
}

bool Display::readTouch(int16_t& x, int16_t& y) {
    if (!_touch.touched()) return false;
    TS_Point p = _touch.getPoint();
    int16_t sx = map(p.x, _xMin, _xMax, 0, 320);
    int16_t sy = map(p.y, _yMin, _yMax, 0, 240);
    if (sx < 0) sx = 0;
    if (sx > 319) sx = 319;
    if (sy < 0) sy = 0;
    if (sy > 239) sy = 239;
    x = sx; y = sy;
    return true;
}
