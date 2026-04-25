#include "UI.h"
#include "UIWidgets.h"
#include <Audio.h>
#include <string.h>

// Static member definition
constexpr char UI::CHARSET[];

void UI::begin(Display* d, Synth* s, PatchStorage* st) {
    disp = d; synth = s; store = st;
    fullRedraw = true;
}

void UI::formatKnobValue(uint8_t idx, char* out, size_t n) {
    const KnobBinding& kb = PAGES[currentPage].knobs[idx];
    if (kb.paramId >= PARAM_COUNT) { out[0] = 0; return; }
    float v = synth->getParam(kb.paramId);
    if (kb.format) kb.format(v, out, n);
    else           fmtGeneric(v, out, n);
}

// ---------------------------------------------------------------------------
// Rename helpers
// ---------------------------------------------------------------------------

uint8_t UI::charsetIndexOf(char c) {
    for (uint8_t i = 0; i < CHARSET_LEN; i++)
        if (CHARSET[i] == c) return i;
    return 0;  // default to space
}

void UI::startRename() {
    strncpy(editName, synth->getPatch().name, 16);
    editName[16] = '\0';
    // Pad to full 16 chars with spaces so every slot is editable
    for (int i = strlen(editName); i < 16; i++) editName[i] = ' ';
    editName[16] = '\0';
    editCursor = 0;
    renaming = true;
    fullRedraw = true;
}

void UI::commitRename() {
    // Trim trailing spaces before saving
    int len = 16;
    while (len > 1 && editName[len - 1] == ' ') len--;
    editName[len] = '\0';
    strncpy(synth->getPatch().name, editName, 16);
    synth->getPatch().name[16] = '\0';
    renaming = false;
    fullRedraw = true;
}

void UI::cancelRename() {
    renaming = false;
    fullRedraw = true;
}

void UI::cycleChar(int8_t direction) {
    uint8_t idx = charsetIndexOf(editName[editCursor]);
    idx = (uint8_t)((idx + CHARSET_LEN + direction) % CHARSET_LEN);
    editName[editCursor] = CHARSET[idx];
    // Redraw just the editor area without full redraw
    drawRenameEditor();
    drawStatusBar();
}

// ---------------------------------------------------------------------------
// Drawing
// ---------------------------------------------------------------------------

void UI::drawHeader() {
    ILI9341_t3& g = disp->tft();
    g.fillRect(0, 0, 320, HEADER_H, Theme::HEADER_BG);

    int x = TAB_MARGIN;
    for (int i = 0; i < TAB_COUNT; i++) {
        UIWidgets::drawTab(g, x, 2, TAB_W, HEADER_H - 4,
                           PAGES[i].name, i == currentPage);
        x += TAB_W + TAB_GAP;
    }
}

void UI::drawStatusBar() {
    ILI9341_t3& g = disp->tft();
    int y = 240 - STATUS_H;
    int dotY = y + (STATUS_H - SB_DOT_W) / 2;

    g.fillRect(0, y, 320, STATUS_H, Theme::HEADER_BG);

    UIWidgets::drawButton(g, 0, y, PATCH_BTN_W, STATUS_H,
                          "PATCH", currentPage == PAGE_PATCH);

    g.setTextColor(Theme::ACCENT_AMBER);
    g.setTextSize(1);
    g.setCursor(PATCH_BTN_W + 4, y + 4);
    g.print(synth->getPatch().name);

    float cpu = AudioProcessorUsage();
    char cpuBuf[8];
    snprintf(cpuBuf, sizeof(cpuBuf), "%2d%%CPU", (int)cpu);
    int cpuX = 320 - SB_RIGHT_MARGIN - SB_CPU_W;
    g.setTextColor(Theme::TEXT_DIM);
    g.setTextSize(1);
    g.setCursor(cpuX, y + 4);
    g.print(cpuBuf);

    bool mask[NUM_VOICES];
    synth->getVoiceActivity(mask);
    int dotsRightX = cpuX - SB_SECTION_GAP;
    int dotsLeftX  = dotsRightX - SB_DOT_BLOCK;
    int dx = dotsLeftX;
    for (int i = 0; i < NUM_VOICES; i++) {
        uint16_t col = mask[i] ? Theme::ACCENT_AMBER : Theme::GRID;
        g.fillRect(dx, dotY, SB_DOT_W, SB_DOT_W, col);
        dx += SB_DOT_W + SB_DOT_GAP;
    }
}

void UI::drawRenameEditor() {
    ILI9341_t3& g = disp->tft();
    int contentY = HEADER_H;
    int contentH = 240 - HEADER_H - STATUS_H;
    g.fillRect(0, contentY, 320, contentH, Theme::BG);

    // Title
    g.setTextColor(Theme::TEXT);
    g.setTextSize(1);
    g.setCursor(8, contentY + 6);
    g.print("RENAME PATCH");

    // Character slots
    int slotY = contentY + NE_ROW_Y - HEADER_H;  // relative to content area
    for (int i = 0; i < 16; i++) {
        int sx = NE_MARGIN_X + i * (NE_SLOT_W + NE_SLOT_GAP);
        bool isCursor = (i == editCursor);
        uint16_t bg  = isCursor ? Theme::ACCENT_AMBER : Theme::PANEL;
        uint16_t fg  = isCursor ? Theme::BG           : Theme::TEXT;
        g.fillRect(sx, slotY, NE_SLOT_W, NE_SLOT_H, bg);
        g.drawRect(sx, slotY, NE_SLOT_W, NE_SLOT_H,
                   isCursor ? Theme::ACCENT_AMBER : Theme::GRID);
        g.setTextColor(fg);
        g.setTextSize(1);
        // Centre the character in the slot (6px wide, 8px tall at textSize 1)
        g.setCursor(sx + (NE_SLOT_W - 6) / 2, slotY + (NE_SLOT_H - 8) / 2);
        g.print(editName[i]);
    }

    // Up/Down arrows above and below the cursor slot to hint interaction
    int cx = NE_MARGIN_X + editCursor * (NE_SLOT_W + NE_SLOT_GAP) + (NE_SLOT_W - 6) / 2;
    g.setTextColor(Theme::TEXT_DIM);
    g.setTextSize(1);
    g.setCursor(cx, slotY - 10);
    g.print("^");
    g.setCursor(cx, slotY + NE_SLOT_H + 2);
    g.print("v");

    // Button row:
    // [CANCEL 80] [◄ 40] [▼ 46] [▲ 46] [► 40] [DONE 64]  total=316 + 4 gaps = 320
    int btnY = 240 - STATUS_H - 26;
    UIWidgets::drawButton(g,   0, btnY,  64, 22, "CANCEL", false);
    UIWidgets::drawButton(g,  65, btnY,  40, 22, "< ",     false);
    UIWidgets::drawButton(g, 106, btnY,  46, 22, " v ",    false);
    UIWidgets::drawButton(g, 153, btnY,  46, 22, " ^ ",    false);
    UIWidgets::drawButton(g, 200, btnY,  40, 22, " >",     false);
    UIWidgets::drawButton(g, 241, btnY,  75, 22, "DONE",   false);
}

void UI::drawPatchPage() {
    ILI9341_t3& g = disp->tft();
    g.fillRect(0, HEADER_H, 320, 240 - HEADER_H - STATUS_H, Theme::BG);

    g.setTextColor(Theme::TEXT);
    g.setTextSize(1);
    g.setCursor(8, HEADER_H + 4);
    char hdr[32];
    snprintf(hdr, sizeof(hdr), "PATCH BANK %d", patchBrowserIndex);
    g.print(hdr);

    for (int i = 0; i < 8; i++) {
        int row = i / 2;
        int col = i % 2;
        int x = 8 + col * 152;
        int y = HEADER_H + 20 + row * 36;
        char label[24];
        Patch tmp;
        uint8_t slot = patchBrowserIndex * 8 + i;
        if (store->load(slot, tmp))
            snprintf(label, sizeof(label), "%03d %s", slot, tmp.name);
        else
            snprintf(label, sizeof(label), "%03d empty", slot);
        UIWidgets::drawButton(g, x, y, 148, 30, label, false);
    }

    UIWidgets::drawButton(g,   8, 240 - STATUS_H - 26, 74, 22, "SAVE",    false);
    UIWidgets::drawButton(g,  84, 240 - STATUS_H - 26, 74, 22, "RENAME",  false);
    UIWidgets::drawButton(g, 160, 240 - STATUS_H - 26, 74, 22, "< PAGE",  false);
    UIWidgets::drawButton(g, 236, 240 - STATUS_H - 26, 80, 22, "PAGE >",  false);
}

void UI::drawKnob(uint8_t idx) {
    const KnobBinding& kb = PAGES[currentPage].knobs[idx];
    if (kb.paramId >= PARAM_COUNT) return;
    int x,y,w,h; knobRect(idx, x,y,w,h);
    char val[12]; formatKnobValue(idx, val, sizeof(val));
    float v = synth->getParam(kb.paramId);
    UIWidgets::drawKnob(disp->tft(), x, y, w, h,
                        kb.label, val, v, idx == selectedKnob);
}

void UI::drawKnobsPage() {
    ILI9341_t3& g = disp->tft();
    g.fillRect(0, HEADER_H, 320, 240 - HEADER_H - STATUS_H, Theme::BG);
    for (int i = 0; i < 8; i++) {
        if (PAGES[currentPage].knobs[i].paramId < PARAM_COUNT)
            drawKnob(i);
    }
}

void UI::drawFull() {
    disp->tft().fillScreen(Theme::BG);
    drawHeader();
    if (currentPage == PAGE_PATCH) {
        if (renaming) drawRenameEditor();
        else          drawPatchPage();
    } else {
        drawKnobsPage();
    }
    drawStatusBar();
    fullRedraw = false;
}

// ---------------------------------------------------------------------------
// Navigation
// ---------------------------------------------------------------------------

void UI::selectPage(PageId p) {
    currentPage = p;
    selectedKnob = -1;
    renaming = false;
    fullRedraw = true;
}

void UI::setSelectedKnob(int8_t idx) {
    if (idx == selectedKnob) return;
    int8_t old = selectedKnob;
    selectedKnob = idx;
    if (old >= 0) drawKnob(old);
    if (idx >= 0) drawKnob(idx);
    drawStatusBar();
}

void UI::hitTestHeader(int16_t x, int16_t y) {
    if (y >= HEADER_H) return;
    int tx = TAB_MARGIN;
    for (int i = 0; i < TAB_COUNT; i++) {
        if (x >= tx && x < tx + TAB_W) {
            selectPage((PageId)i);
            return;
        }
        tx += TAB_W + TAB_GAP;
    }
}

int8_t UI::hitTestKnob(int16_t x, int16_t y) {
    for (int i = 0; i < 8; i++) {
        if (PAGES[currentPage].knobs[i].paramId >= PARAM_COUNT) continue;
        int kx, ky, kw, kh; knobRect(i, kx, ky, kw, kh);
        if (x >= kx && x < kx + kw && y >= ky && y < ky + kh)
            return i;
    }
    return -1;
}

// ---------------------------------------------------------------------------
// Touch handling
// ---------------------------------------------------------------------------

void UI::handleRenameTouch(int16_t x, int16_t y) {
    int btnY = 240 - STATUS_H - 26;

    // Button row hit test
    if (y >= btnY && y < btnY + 22) {
        if      (x <  65)  cancelRename();           // CANCEL
        else if (x < 105)  { if (editCursor > 0)  { editCursor--; drawRenameEditor(); } }  // ◄
        else if (x < 152)  cycleChar(-1);            // ▼ (cycle backward)
        else if (x < 200)  cycleChar(+1);            // ▲ (cycle forward)
        else if (x < 240)  { if (editCursor < 15) { editCursor++; drawRenameEditor(); } }  // ►
        else               commitRename();            // DONE
        return;
    }

    // Tap a character slot to move cursor there
    int slotY = HEADER_H + NE_ROW_Y - HEADER_H;
    if (y >= slotY && y < slotY + NE_SLOT_H) {
        for (int i = 0; i < 16; i++) {
            int sx = NE_MARGIN_X + i * (NE_SLOT_W + NE_SLOT_GAP);
            if (x >= sx && x < sx + NE_SLOT_W) {
                editCursor = i;
                drawRenameEditor();
                return;
            }
        }
    }
}

void UI::handlePatchTouch(int16_t x, int16_t y, bool pressed) {
    if (!pressed) return;

    int btnY = 240 - STATUS_H - 26;
    if (y >= btnY && y < btnY + 22) {
        if (x < 82) {
            // SAVE
            store->save(patchBrowserIndex * 8, synth->getPatch());
            fullRedraw = true;
        } else if (x < 158) {
            // RENAME
            startRename();
        } else if (x < 234) {
            // < PAGE
            if (patchBrowserIndex > 0) { patchBrowserIndex--; fullRedraw = true; }
        } else {
            // PAGE >
            if (patchBrowserIndex < 15) { patchBrowserIndex++; fullRedraw = true; }
        }
        return;
    }

    for (int i = 0; i < 8; i++) {
        int row = i / 2, col = i % 2;
        int sx = 8 + col * 152;
        int sy = HEADER_H + 20 + row * 36;
        if (x >= sx && x < sx + 148 && y >= sy && y < sy + 30) {
            loadPatchAt(patchBrowserIndex * 8 + i);
            return;
        }
    }
}

void UI::handleTouch(int16_t x, int16_t y, bool pressed) {
    if (!pressed) {
        isDragging = false;
        return;
    }

    if (!isDragging) {
        // Header tab strip
        if (y < HEADER_H) {
            hitTestHeader(x, y);
            return;
        }

        // PATCH button in the status bar (bottom-left)
        int statusY = 240 - STATUS_H;
        if (y >= statusY && x < PATCH_BTN_W) {
            selectPage(PAGE_PATCH);
            return;
        }

        if (currentPage == PAGE_PATCH) {
            if (renaming) handleRenameTouch(x, y);
            else          handlePatchTouch(x, y, true);
            return;
        }

        int8_t k = hitTestKnob(x, y);
        if (k >= 0) {
            setSelectedKnob(k);
            isDragging = true;
            dragStartY = y;
            uint8_t pid = PAGES[currentPage].knobs[k].paramId;
            dragStartValue = synth->getParam(pid);
        }
        return;
    }

    if (selectedKnob >= 0) {
        uint8_t pid = PAGES[currentPage].knobs[selectedKnob].paramId;
        if (pid >= PARAM_COUNT) return;
        float delta = (dragStartY - y) / 200.0f;
        float v = dragStartValue + delta;
        if (v < 0.0f) v = 0.0f;
        if (v > 1.0f) v = 1.0f;
        synth->setParam(pid, v);
        drawKnob(selectedKnob);
        drawStatusBar();
    }
}

void UI::loadPatchAt(uint8_t slot) {
    Patch p;
    if (store->load(slot, p)) {
        synth->loadPatch(p);
        fullRedraw = true;
    }
}

// ---------------------------------------------------------------------------
// Main loop
// ---------------------------------------------------------------------------

void UI::poll() {
    if (disp->touched()) {
        int16_t x, y;
        if (disp->readTouch(x, y)) {
            handleTouch(x, y, true);
        }
    } else {
        if (isDragging) handleTouch(0, 0, false);
    }

    if (fullRedraw) drawFull();

    if (synth->consumeParamChanged() && !fullRedraw) {
        if (currentPage != PAGE_PATCH) {
            for (int i = 0; i < 8; i++) {
                if (PAGES[currentPage].knobs[i].paramId < PARAM_COUNT)
                    drawKnob(i);
            }
            drawStatusBar();
        }
    }
}
