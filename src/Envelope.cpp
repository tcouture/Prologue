#include "Envelope.h"

void Envelope::setADSR(float a, float d, float s, float r) {
    attackRate   = 1.0f / (a * sampleRate + 1.0f);
    decayRate    = 1.0f / (d * sampleRate + 1.0f);
    sustainLevel = s;
    releaseRate  = 1.0f / (r * sampleRate + 1.0f);
}

void Envelope::noteOn()  { stage = ATTACK; }
void Envelope::noteOff() { if (stage != IDLE) stage = RELEASE; }

float Envelope::process() {
    switch (stage) {
        case ATTACK:
            level += attackRate;
            if (level >= 1.0f) { level = 1.0f; stage = DECAY; }
            break;
        case DECAY:
            level -= decayRate * (level - sustainLevel);
            if (level <= sustainLevel + 0.001f) { level = sustainLevel; stage = SUSTAIN; }
            break;
        case SUSTAIN: level = sustainLevel; break;
        case RELEASE:
            level -= releaseRate * level;
            if (level < 0.001f) { level = 0.0f; stage = IDLE; }
            break;
        default: break;
    }
    return level;
}
