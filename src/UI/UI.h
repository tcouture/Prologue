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

    static constexpr int HEADER_H    = 24;
    static constexpr int STATUS_H    = 16;
    static constexpr int TAB_COUNT   = PAGE_COUNT - 1;  // PAGE_PATCH excluded from header
    static constexpr int TAB_W       = 51;              // (320 - 2 margins - 5×2px gaps) / 6
    static constexpr int TAB_GAP     = 2;
    static constexpr int TAB_MARGIN  = 1;

    static constexpr int PATCH_BTN_W = 52;              // PATCH button in status bar

    // Status bar right-side metrics (all measured from right edge, rightward)
    static constexpr int SB_RIGHT_MARGIN = 4;           // px from right edge
    static constexpr int SB_CPU_W        = 42;          // 7 chars × 6px ("CPU:XX%")
    static constexpr int SB_DOT_W        = 6;           // voice dot diameter
    static constexpr int SB_DOT_GAP      = 2;           // gap between dots
    static constexpr int SB_DOT_BLOCK    = NUM_VOICES * SB_DOT_W + (NUM_VOICES - 1) * SB_DOT_GAP;
    static constexpr int SB_SECTION_GAP  = 4;           // gap between cpu/dots and dots/name

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
