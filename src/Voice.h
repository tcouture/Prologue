#pragma once
#include "Oscillator.h"
#include "Filter.h"
#include "Envelope.h"
#include "LFO.h"
#include "ParamSmoother.h"
#include "Params.h"

class Voice {
public:
    void init(float sr);
    void noteOn(uint8_t note, uint8_t vel);
    void noteOff();
    bool    isActive()  const { return ampEG.isActive(); }
    uint8_t getNote()   const { return currentNote; }
    void setParams(const float* params);
    void setPitchBend(float semis)      { pitchBendSemis = semis; }
    void setPortamentoTime(float sec)   { portaTimeSec = sec; }
    void setUnisonDetune(float cents)   { unisonDetuneCents = cents; }  // per-voice static detune
    void renderBlock(float* out, uint32_t n);

private:
    float sampleRate = 44100.0f;

    Oscillator vco1, vco2;
    Filter     filter;
    Envelope   ampEG, modEG;
    LFO        lfo;

    uint8_t currentNote  = 0;
    uint8_t targetNote   = 0;
    float   velocity     = 1.0f;

    float baseFreqHz     = 440.0f;
    float targetFreqHz   = 440.0f;
    float portaCoeff     = 1.0f;
    float portaTimeSec   = 0.0f;
    float pitchBendSemis = 0.0f;

    // Per-voice static detune for unison mode (cents, set by VoiceManager)
    float unisonDetuneCents = 0.0f;

    float vco2DetuneRatio = 1.003f;
    int   vco1OctShift    = 0;
    int   vco2OctShift    = 0;

    float baseCutoff  = 2000.0f;
    float modEGAmount = 0.0f;
    float keyTrack    = 0.0f;

    ParamSmoother mix1Smooth, mix2Smooth, mixNoisSmooth;
    ParamSmoother cutoffSmooth;
    ParamSmoother pitchBendSmooth;

    LFOShape lfoShape = LFO_SINE;
    float    lfoDepth = 0.0f;
    uint8_t  lfoDest  = LFO_DEST_PITCH;

    // FM (VCO2 modulates VCO1 phase)
    float fmDepth = 0.0f;

    // White noise — per-voice LCG so voices are uncorrelated
    uint32_t noiseSeed = 22222u;
    inline float nextNoise() {
        noiseSeed = noiseSeed * 1664525u + 1013904223u;
        return (int32_t)noiseSeed / 2147483648.0f;
    }
};
