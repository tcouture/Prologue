#pragma once
#include "Display.h"
#include "../Synth.h"
#include "../PatchStorage.h"
#include "UIPages.h"

class UI {
public:
    void begin(Display* d, Synth* s, PatchStorage* store);
    void poll();

private:
    Display*      disp  = nullptr;
    Synth*        synth = nullptr;
    PatchStorage* store = nullptr;

    PageId currentPage = PAGE_OSC;
    int8_t selectedKnob = -1;

    bool    isDragging = false;
    int16_t dragStartY = 0;
    float   dragStartValue = 0.0f;

    static constexpr int HEADER_H = 24;
    static constexpr int STATUS_H = 16;
    static constexpr int TAB_COUNT = PAGE_COUNT;

    int8_t patchBrowserIndex = 0;
    bool   fullRedraw = true;

    void drawFull();
    void drawHeader();
    void drawStatusBar();
    void drawKnobsPage();
    void drawKnob(uint8_t idx);
    void drawPatchPage();

    void handleTouch(int16_t x, int16_t y, bool pressed);
    void hitTestHeader(int16_t x, int16_t y);
    int8_t hitTestKnob(int16_t x, int16_t y);
    void selectPage(PageId p);
    void setSelectedKnob(int8_t idx);

    void handlePatchTouch(int16_t x, int16_t y, bool pressed);
    void loadPatchAt(uint8_t slot);

    void knobRect(uint8_t idx, int& x, int& y, int& w, int& h) {
        int col = idx % 4;
        int row = idx / 4;
        w = 76;
        h = 90;
        x = 4 + col * (w + 4);
        y = HEADER_H + 4 + row * (h + 4);
    }

    void formatKnobValue(uint8_t idx, char* out, size_t n);
};
