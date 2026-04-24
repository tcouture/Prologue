#pragma once
#include <Arduino.h>
#include <math.h>

enum LFOShape { LFO_SINE = 0, LFO_TRI, LFO_SQR, LFO_SAW, LFO_SH };

class LFO {
public:
    void init(float sr) { sampleRate = sr; }
    void setShape(LFOShape s) { shape = s; }
    void setRate(float hz)    { phaseInc = hz / sampleRate; }
    void reset()              { phase = 0.0f; }

    float process() {
        phase += phaseInc;
        if (phase >= 1.0f) {
            phase -= 1.0f;
            shValue = (random(0, 2000) - 1000) / 1000.0f;
        }
        switch (shape) {
            case LFO_SINE: return sinf(phase * 6.2831853f);
            case LFO_TRI:  return phase < 0.5f
                                ? (phase * 4.0f - 1.0f)
                                : (3.0f - phase * 4.0f);
            case LFO_SQR:  return phase < 0.5f ? 1.0f : -1.0f;
            case LFO_SAW:  return 2.0f * phase - 1.0f;
            case LFO_SH:   return shValue;
        }
        return 0.0f;
    }

private:
    float sampleRate = 44100.0f;
    float phase = 0.0f;
    float phaseInc = 0.0f;
    float shValue = 0.0f;
    LFOShape shape = LFO_SINE;
};
