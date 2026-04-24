#pragma once
#include <ILI9341_t3.h>
#include <XPT2046_Touchscreen.h>
#include "../Pins.h"
#include "Theme.h"

class Display {
public:
    void begin();
    ILI9341_t3& tft() { return _tft; }
    XPT2046_Touchscreen& touch() { return _touch; }

    bool readTouch(int16_t& x, int16_t& y);
    bool touched();

    // Adjust these after running touch calibration
    int16_t _xMin = 300, _xMax = 3800;
    int16_t _yMin = 300, _yMax = 3800;

private:
    ILI9341_t3           _tft { TFT_CS, TFT_DC, TFT_RST, SPI_MOSI, SPI_SCK, SPI_MISO };
    XPT2046_Touchscreen  _touch { TOUCH_CS, TOUCH_IRQ };
};
