#include "Filter.h"
#include <math.h>

void Filter::updateCoeffs() {
    if (cutoff < 20.0f) cutoff = 20.0f;
    if (cutoff > sampleRate * 0.45f) cutoff = sampleRate * 0.45f;
    g = tanf(3.14159265f * cutoff / sampleRate);
    k = 2.0f - 2.0f * res;
    a1 = 1.0f / (1.0f + g * (g + k));
}

float Filter::processLP(float in) {
    in = tanhf(in * drive);
    float v3 = in - ic2eq;
    float v1 = a1 * (g * v3 + ic1eq);
    float v2 = ic2eq + g * v1;
    ic1eq = 2.0f * v1 - ic1eq;
    ic2eq = 2.0f * v2 - ic2eq;
    return v2;
}
