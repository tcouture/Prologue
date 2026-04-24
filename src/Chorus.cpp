#include "Chorus.h"
#include <math.h>

void Chorus::process(float* L, float* R, uint32_t n) {
    float baseDelay = 0.015f * sampleRate;
    float modDepth  = 0.005f * sampleRate * depth;

    for (uint32_t i = 0; i < n; i++) {
        float in = (L[i] + R[i]) * 0.5f;
        buffer[writeIdx] = in;

        lfoPhase += lfoInc;
        if (lfoPhase >= 1.0f) lfoPhase -= 1.0f;

        float lfoL = sinf(lfoPhase * 6.2831853f);
        float lfoR = sinf((lfoPhase + 0.25f) * 6.2831853f);

        float dL = baseDelay + lfoL * modDepth;
        float dR = baseDelay + lfoR * modDepth;

        float wetL = readInterp(dL);
        float wetR = readInterp(dR);

        float mix = depth * 0.5f;
        L[i] = L[i] * (1.0f - mix) + wetL * mix;
        R[i] = R[i] * (1.0f - mix) + wetR * mix;

        writeIdx = (writeIdx + 1) % BUF_LEN;
    }
}
