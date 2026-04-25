#pragma once
#include <Arduino.h>

enum WaveShape { WAVE_SAW = 0, WAVE_TRI, WAVE_SQR };

class Oscillator {
public:
    void  init(float sampleRate);
    void  setFrequency(float hz);
    void  setShape(WaveShape w)    { shape = w; }
    void  setShapeMod(float s)     { shapeMod = s; }
    void  setPhaseOffset(float o)  { phaseOffset = o; }  // FM: normalised [0,1) phase offset
    void  reset()                  { phase = 0.0f; }
    float process();

private:
    float     sampleRate  = 44100.0f;
    float     phase       = 0.0f;
    float     phaseInc    = 0.0f;
    float     phaseOffset = 0.0f;
    WaveShape shape       = WAVE_SAW;
    float     shapeMod    = 0.5f;
    float     triState    = 0.0f;

    static float polyBlep(float t, float dt);
};
