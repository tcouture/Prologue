#include "UIPages.h"
#include <Arduino.h>
#include <math.h>

void fmtGeneric(float v, char* o, size_t n) {
    snprintf(o, n, "%d", (int)(v * 127.0f));
}
void fmtWave(float v, char* o, size_t n) {
    static const char* names[] = { "SAW", "TRI", "SQR" };
    snprintf(o, n, "%s", names[(int)(v * 2.99f)]);
}
void fmtLfoWave(float v, char* o, size_t n) {
    static const char* names[] = { "SIN", "TRI", "SQR", "SAW", "S&H" };
    snprintf(o, n, "%s", names[(int)(v * 4.99f)]);
}
void fmtLfoDest(float v, char* o, size_t n) {
    static const char* names[] = { "PITCH", "SHAPE", "CUTOFF", "AMP" };
    snprintf(o, n, "%s", names[(int)(v * 3.99f)]);
}
void fmtOctave(float v, char* o, size_t n) {
    snprintf(o, n, "%+d", (int)(v * 4.99f) - 2);
}
void fmtMs(float v, char* o, size_t n) {
    float ms = 1.0f * powf(10000.0f, v);
    if (ms < 1000.0f) snprintf(o, n, "%dms", (int)ms);
    else              snprintf(o, n, "%.1fs", ms / 1000.0f);
}
void fmtHz(float v, char* o, size_t n) {
    float hz = 20.0f * powf(1000.0f, v);
    if (hz < 1000.0f) snprintf(o, n, "%dHz", (int)hz);
    else              snprintf(o, n, "%.1fk", hz / 1000.0f);
}
void fmtPercent(float v, char* o, size_t n) {
    snprintf(o, n, "%d%%", (int)(v * 100.0f));
}
void fmtVoiceMode(float v, char* o, size_t n) {
    static const char* names[] = { "POLY", "UNISON", "MONO" };
    snprintf(o, n, "%s", names[(int)(v * 2.99f)]);
}
void fmtPortamento(float v, char* o, size_t n) {
    if (v < 0.01f) { snprintf(o, n, "OFF"); return; }
    float ms = 1.0f * powf(2000.0f, v);
    if (ms < 1000.0f) snprintf(o, n, "%dms", (int)ms);
    else              snprintf(o, n, "%.1fs", ms / 1000.0f);
}
void fmtFmDepth(float v, char* o, size_t n) {
    snprintf(o, n, "%.2f", v * 4.0f);
}
void fmtCents(float v, char* o, size_t n) {
    snprintf(o, n, "%dct", (int)(v * 50.0f));
}

const PageDef PAGES[PAGE_COUNT] = {
    { "OSC", {
        { P_VCO1_WAVE,  "VCO1 WAVE",  fmtWave    },
        { P_VCO1_SHAPE, "VCO1 SHAPE", fmtPercent },
        { P_VCO1_OCT,   "VCO1 OCT",   fmtOctave  },
        { P_VCO1_PITCH, "VCO1 PITCH", fmtGeneric },
        { P_VCO2_WAVE,  "VCO2 WAVE",  fmtWave    },
        { P_VCO2_SHAPE, "VCO2 SHAPE", fmtPercent },
        { P_VCO2_OCT,   "VCO2 OCT",   fmtOctave  },
        { P_VCO2_PITCH, "VCO2 PITCH", fmtGeneric },
    }},
    { "FILTER", {
        { P_FILT_CUTOFF,    "CUTOFF",    fmtHz      },
        { P_FILT_RESO,      "RESO",      fmtPercent },
        { P_FILT_DRIVE,     "DRIVE",     fmtPercent },
        { P_FILT_KEYTRK,    "KEYTRK",    fmtPercent },
        { P_MIX_VCO1,       "MIX VCO1",  fmtPercent },
        { P_MIX_VCO2,       "MIX VCO2",  fmtPercent },
        { P_MIX_MULTI,      "NOISE LVL", fmtPercent },
        { P_VCO2_CROSS_MOD, "FM DEPTH",  fmtFmDepth },
    }},
    { "ENV", {
        { P_AEG_A,   "AMP A",   fmtMs      },
        { P_AEG_D,   "AMP D",   fmtMs      },
        { P_AEG_S,   "AMP S",   fmtPercent },
        { P_AEG_R,   "AMP R",   fmtMs      },
        { P_MEG_A,   "MOD A",   fmtMs      },
        { P_MEG_D,   "MOD D",   fmtMs      },
        { P_MEG_S,   "MOD S",   fmtPercent },
        { P_MEG_AMT, "MOD AMT", fmtPercent },
    }},
    { "LFO", {
        { P_LFO_WAVE,  "LFO WAVE", fmtLfoWave },
        { P_LFO_RATE,  "LFO RATE", fmtHz      },
        { P_LFO_DEPTH, "LFO DPT",  fmtPercent },
        { P_LFO_DEST,  "LFO DEST", fmtLfoDest },
        { PARAM_COUNT, "",         nullptr    },
        { PARAM_COUNT, "",         nullptr    },
        { PARAM_COUNT, "",         nullptr    },
        { PARAM_COUNT, "",         nullptr    },
    }},
    { "FX", {
        { P_FX_MOD_TYPE,    "MOD TYPE",  fmtGeneric },
        { P_FX_MOD_DEPTH,   "MOD DPT",   fmtPercent },
        { P_FX_DELAY_TIME,  "DLY TIME",  fmtMs      },
        { P_FX_DELAY_FB,    "DLY FB",    fmtPercent },
        { P_FX_DELAY_MIX,   "DLY MIX",   fmtPercent },
        { P_FX_REVERB_SIZE, "REV SIZE",  fmtPercent },
        { P_FX_REVERB_MIX,  "REV MIX",   fmtPercent },
        { P_MASTER_VOL,     "MASTER",    fmtPercent },
    }},
    { "GLOBAL", {
        { P_PORTAMENTO,    "PORTAMENTO",      fmtPortamento },
        { P_VOICE_MODE,    "VOICE MODE", fmtVoiceMode  },
        { P_UNISON_DETUNE, "UNI DETUNE", fmtCents      },
        { PARAM_COUNT,     "",           nullptr },
        { PARAM_COUNT,     "",           nullptr },
        { PARAM_COUNT,     "",           nullptr },
        { PARAM_COUNT,     "",           nullptr },
        { PARAM_COUNT,     "",           nullptr },
    }},
    { "PATCH", {
        { PARAM_COUNT, "", nullptr },
    }},
};
