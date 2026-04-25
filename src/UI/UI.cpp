#include "UI.h"
#include "UIWidgets.h"
#include <Audio.h>   // AudioProcessorUsage()
#include <string.h>

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
    int dotY = y + (STATUS_H - SB_DOT_W) / 2;  // vertically centre dots

    g.fillRect(0, y, 320, STATUS_H, Theme::HEADER_BG);

    // --- PATCH button (bottom-left) ---
    UIWidgets::drawButton(g, 0, y, PATCH_BTN_W, STATUS_H,
                          "PATCH", currentPage == PAGE_PATCH);

    // --- Patch name (amber, immediately right of PATCH button) ---
    g.setTextColor(Theme::ACCENT_AMBER);
    g.setTextSize(1);
    g.setCursor(PATCH_BTN_W + 4, y + 4);
    g.print(synth->getPatch().name);

    // --- CPU usage (right-aligned at far right) ---
    float cpu = AudioProcessorUsage();
    char cpuBuf[8];
    snprintf(cpuBuf, sizeof(cpuBuf), "%2d%%CPU", (int)cpu);  // e.g. "34%CPU"
    int cpuX = 320 - SB_RIGHT_MARGIN - SB_CPU_W;
    g.setTextColor(Theme::TEXT_DIM);
    g.setTextSize(1);
    g.setCursor(cpuX, y + 4);
    g.print(cpuBuf);

    // --- Voice activity dots (immediately left of CPU) ---
    bool mask[NUM_VOICES];
    synth->getVoiceActivity(mask);
    int dotsRightX = cpuX - SB_SECTION_GAP;          // right edge of dot block
    int dotsLeftX  = dotsRightX - SB_DOT_BLOCK;      // left edge of dot block
    int dx = dotsLeftX;
    for (int i = 0; i < NUM_VOICES; i++) {
        uint16_t col = mask[i] ? Theme::ACCENT_AMBER : Theme::GRID;
        g.fillRect(dx, dotY, SB_DOT_W, SB_DOT_W, col);
        dx += SB_DOT_W + SB_DOT_GAP;
    }
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

    UIWidgets::drawButton(g, 8,   240 - STATUS_H - 26, 100, 22, "SAVE CUR", false);
    UIWidgets::drawButton(g, 114, 240 - STATUS_H - 26, 100, 22, "< PAGE",   false);
    UIWidgets::drawButton(g, 218, 240 - STATUS_H - 26, 94,  22, "PAGE >",   false);
}

void UI::drawFull() {
    disp->tft().fillScreen(Theme::BG);
    drawHeader();
    if (currentPage == PAGE_PATCH) drawPatchPage();
    else                           drawKnobsPage();
    drawStatusBar();
    fullRedraw = false;
}

void UI::selectPage(PageId p) {
    currentPage = p;
    selectedKnob = -1;
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
            handlePatchTouch(x, y, true);
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

void UI::handlePatchTouch(int16_t x, int16_t y, bool pressed) {
    if (!pressed) return;

    int btnY = 240 - STATUS_H - 26;
    if (y >= btnY && y < btnY + 22) {
        if (x < 108) {
            store->save(patchBrowserIndex * 8, synth->getPatch());
            fullRedraw = true;
        } else if (x < 214) {
            if (patchBrowserIndex > 0) { patchBrowserIndex--; fullRedraw = true; }
        } else {
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

void UI::loadPatchAt(uint8_t slot) {
    Patch p;
    if (store->load(slot, p)) {
        synth->loadPatch(p);
        fullRedraw = true;
    }
}

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
