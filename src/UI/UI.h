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
    static constexpr int TAB_COUNT   = PAGE_COUNT - 1;
    static constexpr int TAB_W       = 51;
    static constexpr int TAB_GAP     = 2;
    static constexpr int TAB_MARGIN  = 1;
    static constexpr int PATCH_BTN_W = 52;

    static constexpr int SB_RIGHT_MARGIN = 4;
    static constexpr int SB_CPU_W        = 42;
    static constexpr int SB_DOT_W        = 6;
    static constexpr int SB_DOT_GAP      = 2;
    static constexpr int SB_DOT_BLOCK    = NUM_VOICES * SB_DOT_W + (NUM_VOICES - 1) * SB_DOT_GAP;
    static constexpr int SB_SECTION_GAP  = 4;

    // Patch name editor state
    bool    renaming = false;
    char    editName[17];       // working copy during rename
    uint8_t editCursor = 0;     // which character slot is selected (0-15)

    // Character set cycled by ▲/▼: A-Z, 0-9, space, common punctuation
    static constexpr char CHARSET[] =
        " ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-_!?";
    static constexpr uint8_t CHARSET_LEN = 42;

    // Name editor geometry
    static constexpr int NE_SLOT_W    = 17;  // width of each char slot
    static constexpr int NE_SLOT_GAP  = 1;
    static constexpr int NE_SLOT_H    = 20;
    static constexpr int NE_MARGIN_X  = 16;  // left edge of slot row
    static constexpr int NE_ROW_Y     = 68;  // y of char slot row (inside content area)

    int8_t patchBrowserIndex = 0;
    bool   fullRedraw = true;

    void drawFull();
    void drawHeader();
    void drawStatusBar();
    void drawKnobsPage();
    void drawKnob(uint8_t idx);
    void drawPatchPage();
    void drawRenameEditor();    // drawn in place of normal patch page content

    void handleTouch(int16_t x, int16_t y, bool pressed);
    void hitTestHeader(int16_t x, int16_t y);
    int8_t hitTestKnob(int16_t x, int16_t y);
    void selectPage(PageId p);
    void setSelectedKnob(int8_t idx);

    void handlePatchTouch(int16_t x, int16_t y, bool pressed);
    void handleRenameTouch(int16_t x, int16_t y);
    void loadPatchAt(uint8_t slot);

    void startRename();
    void commitRename();
    void cancelRename();
    void cycleChar(int8_t direction);   // +1 or -1
    uint8_t charsetIndexOf(char c);

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
