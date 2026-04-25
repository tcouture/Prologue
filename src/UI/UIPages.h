#pragma once
#include "../Params.h"

enum PageId : uint8_t {
    PAGE_OSC = 0,
    PAGE_FILTER,
    PAGE_ENV,
    PAGE_LFO,
    PAGE_FX,
    PAGE_GLOBAL,
    PAGE_PATCH,
    PAGE_COUNT
};

struct KnobBinding {
    uint8_t     paramId;
    const char* label;
    void (*format)(float v01, char* out, size_t n);
};

struct PageDef {
    const char* name;
    KnobBinding knobs[8];
};

extern const PageDef PAGES[PAGE_COUNT];

void fmtGeneric   (float v, char* o, size_t n);
void fmtWave      (float v, char* o, size_t n);
void fmtLfoWave   (float v, char* o, size_t n);
void fmtLfoDest   (float v, char* o, size_t n);
void fmtOctave    (float v, char* o, size_t n);
void fmtMs        (float v, char* o, size_t n);
void fmtHz        (float v, char* o, size_t n);
void fmtPercent   (float v, char* o, size_t n);
void fmtVoiceMode (float v, char* o, size_t n);
void fmtPortamento(float v, char* o, size_t n);
void fmtFmDepth   (float v, char* o, size_t n);
void fmtCents     (float v, char* o, size_t n);
