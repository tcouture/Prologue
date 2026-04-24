#pragma once
#include <math.h>

class ParamSmoother {
public:
    void init(float sampleRate, float timeMs = 10.0f) {
        float tc = timeMs * 0.001f;
        coeff = 1.0f - expf(-1.0f / (tc * sampleRate));
    }
    void setTarget(float t) { target = t; }
    void snap(float v)      { target = current = v; }
    float process() {
        current += coeff * (target - current);
        return current;
    }
    float value() const { return current; }
private:
    float target = 0.0f;
    float current = 0.0f;
    float coeff = 0.01f;
};
