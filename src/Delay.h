#pragma once
#include <Arduino.h>
#include <string.h>

class Delay {
public:
    void init(float sr) {
        sampleRate = sr;
        memset(bufL, 0, sizeof(bufL));
        memset(bufR, 0, sizeof(bufR));
        writeIdx = 0;
    }
    void setTime(float sec)    { targetDelaySamples = sec * sampleRate; }
    void setFeedback(float fb) { feedback = fb * 0.85f; }
    void setMix(float m)       { mix = m; }
    void process(float* L, float* R, uint32_t n);

private:
    static constexpr int BUF_LEN = 32768;
    float sampleRate = 44100.0f;
    float bufL[BUF_LEN], bufR[BUF_LEN];
    int writeIdx = 0;
    float targetDelaySamples = 11025.0f;
    float currentDelaySamples = 11025.0f;
    float feedback = 0.3f;
    float mix = 0.0f;
};
