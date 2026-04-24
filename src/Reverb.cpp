#include "Reverb.h"

void Reverb::init(float sr) {
    (void)sr;
    for (int i = 0; i < NUM_COMBS; i++) {
        comb[i] = new float[combLen[i]]();
    }
    for (int i = 0; i < NUM_APS; i++) {
        ap[i] = new float[apLen[i]]();
    }
    updateParams();
}

void Reverb::process(float* L, float* R, uint32_t n) {
    for (uint32_t s = 0; s < n; s++) {
        float in = (L[s] + R[s]) * 0.5f * 0.015f;
        float out = 0.0f;

        for (int i = 0; i < NUM_COMBS; i++) {
            float y = comb[i][combIdx[i]];
            combLP[i] = y * (1.0f - damp) + combLP[i] * damp;
            comb[i][combIdx[i]] = in + combLP[i] * feedback;
            combIdx[i] = (combIdx[i] + 1) % combLen[i];
            out += y;
        }
        out *= 0.125f;

        for (int i = 0; i < NUM_APS; i++) {
            float y = ap[i][apIdx[i]];
            float x = out + y * 0.5f;
            ap[i][apIdx[i]] = x;
            apIdx[i] = (apIdx[i] + 1) % apLen[i];
            out = y - x;
        }

        L[s] = L[s] * (1.0f - mix) + out * mix;
        R[s] = R[s] * (1.0f - mix) + out * mix;
    }
}
