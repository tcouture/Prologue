#include "Oscillator.h"

void Oscillator::init(float sr) { sampleRate = sr; }

void Oscillator::setFrequency(float hz) {
    if (hz < 0.0f) hz = 0.0f;
    phaseInc = hz / sampleRate;
}

float Oscillator::polyBlep(float t, float dt) {
    if (t < dt)        { t /= dt;         return  t + t - t*t - 1.0f; }
    if (t > 1.0f - dt) { t = (t-1.0f)/dt; return  t*t + t + t + 1.0f; }
    return 0.0f;
}

float Oscillator::process() {
    // Apply FM phase offset, wrapped to [0,1)
    float t = phase + phaseOffset;
    if (t >= 1.0f) t -= (int)t + 1;
    if (t <  0.0f) t += (int)(-t) + 1;
    float dt  = phaseInc;
    float out = 0.0f;

    switch (shape) {
        case WAVE_SAW:
            out = 2.0f * t - 1.0f;
            out -= polyBlep(t, dt);
            break;
        case WAVE_SQR: {
            float pw = 0.5f + (shapeMod - 0.5f) * 0.9f;
            if (pw < 0.05f) pw = 0.05f;
            if (pw > 0.95f) pw = 0.95f;
            out = (t < pw) ? 1.0f : -1.0f;
            out += polyBlep(t, dt);
            float t2 = t + (1.0f - pw);
            if (t2 >= 1.0f) t2 -= 1.0f;
            out -= polyBlep(t2, dt);
            break;
        }
        case WAVE_TRI: {
            float sq = (t < 0.5f) ? 1.0f : -1.0f;
            sq += polyBlep(t, dt);
            float t2 = t + 0.5f; if (t2 >= 1.0f) t2 -= 1.0f;
            sq -= polyBlep(t2, dt);
            triState += 4.0f * dt * sq;
            triState *= 0.999f;
            out = triState;
            break;
        }
    }

    phase += phaseInc;
    if (phase >= 1.0f) phase -= 1.0f;
    phaseOffset = 0.0f;  // consumed each sample; caller sets fresh every tick
    return out;
}
