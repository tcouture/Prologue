#pragma once

class Envelope {
public:
    enum Stage { IDLE, ATTACK, DECAY, SUSTAIN, RELEASE };
    void init(float sr) { sampleRate = sr; }
    void setADSR(float a, float d, float s, float r);
    void noteOn();
    void noteOff();
    float process();
    bool isActive() const { return stage != IDLE; }

private:
    float sampleRate = 44100.0f;
    float attackRate = 0, decayRate = 0, releaseRate = 0, sustainLevel = 0.7f;
    float level = 0.0f;
    Stage stage = IDLE;
};
