#pragma once
#include <Arduino.h>

class Reverb {
public:
    void init(float sr);
    void setSize(float s) { roomSize = 0.28f + s * 0.7f; updateParams(); }
    void setMix(float m)  { mix = m; }
    void process(float* L, float* R, uint32_t n);

private:
    static constexpr int NUM_COMBS = 8;
    static constexpr int NUM_APS   = 4;
    const int combLen[NUM_COMBS] = {1116,1188,1277,1356,1422,1491,1557,1617};
    const int apLen  [NUM_APS  ] = {556, 441, 341, 225};

    float* comb[NUM_COMBS] = {nullptr};
    float* ap  [NUM_APS  ] = {nullptr};
    int    combIdx[NUM_COMBS] = {0};
    int    apIdx  [NUM_APS  ] = {0};
    float  combLP [NUM_COMBS] = {0};

    float damp = 0.2f;
    float roomSize = 0.8f;
    float feedback = 0.84f;
    float mix = 0.0f;

    void updateParams() { feedback = roomSize * 0.28f + 0.7f; }
};
