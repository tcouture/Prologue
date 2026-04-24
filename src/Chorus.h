#pragma once
#include <Arduino.h>
#include <string.h>

class Chorus {
public:
    void init(float sr) {
        sampleRate = sr;
        memset(buffer, 0, sizeof(buffer));
        writeIdx = 0;
        lfoPhase = 0.0f;
    }
    void setDepth(float d01) { depth = d01; }
    void setRate(float hz)   { lfoInc = hz / sampleRate; }
    void process(float* L, float* R, uint32_t n);

private:
    static constexpr int BUF_LEN = 4096;
    float sampleRate = 44100.0f;
    float buffer[BUF_LEN];
    int writeIdx = 0;
    float lfoPhase = 0.0f;
    float lfoInc = 0.0f;
    float depth = 0.0f;

    float readInterp(float delaySamples) {
        float rp = (float)writeIdx - delaySamples;
        while (rp < 0) rp += BUF_LEN;
        int i0 = (int)rp;
        float frac = rp - i0;
        int i1 = (i0 + 1) % BUF_LEN;
        return buffer[i0] + frac * (buffer[i1] - buffer[i0]);
    }
};
