#pragma once
#include <ILI9341_t3.h>
#include "Theme.h"

namespace UIWidgets {
    void drawKnob(ILI9341_t3& g, int x, int y, int w, int h,
                  const char* label, const char* valueStr,
                  float value01, bool selected);
    void drawTab(ILI9341_t3& g, int x, int y, int w, int h,
                 const char* text, bool active);
    void drawButton(ILI9341_t3& g, int x, int y, int w, int h,
                    const char* text, bool pressed);
}
