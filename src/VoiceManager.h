#pragma once
#include "Voice.h"
#include "Params.h"

class VoiceManager {
public:
    void init(float sr) {
        for (int i = 0; i < MAX_VOICES; i++) voices[i].init(sr);
    }
    void noteOn(uint8_t note, uint8_t vel);
    void noteOff(uint8_t note);
    void allNotesOff();
    void renderBlock(float* out, uint32_t n);
    void setParams(const float* p) {
        for (int i = 0; i < NUM_VOICES; i++) voices[i].setParams(p);
    }
    void setPitchBend(float bend01);

private:
    Voice voices[MAX_VOICES];
    uint8_t findFreeVoice();
    uint32_t voiceAge[MAX_VOICES] = {0};
    uint32_t ageCounter = 0;
};
