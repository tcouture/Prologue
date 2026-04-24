#include "Delay.h"

void Delay::process(float* L, float* R, uint32_t n) {
    for (uint32_t i = 0; i < n; i++) {
        currentDelaySamples += 0.0005f * (targetDelaySamples - currentDelaySamples);

        int readPos = writeIdx - (int)currentDelaySamples;
        while (readPos < 0) readPos += BUF_LEN;

        float dL = bufL[readPos];
        float dR = bufR[readPos];

        bufL[writeIdx] = R[i] + dL * feedback;
        bufR[writeIdx] = L[i] + dR * feedback;

        L[i] = L[i] * (1.0f - mix) + dL * mix;
        R[i] = R[i] * (1.0f - mix) + dR * mix;

        writeIdx = (writeIdx + 1) % BUF_LEN;
    }
}
