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
        voiceMode    = (VoiceMode)(int)(p[P_VOICE_MODE] * 2.99f);
        unisonDetune = p[P_UNISON_DETUNE] * 50.0f;  // 0-1 -> 0-50 cents max spread
        for (int i = 0; i < NUM_VOICES; i++) voices[i].setParams(p);
        applyUnisonDetune();
    }

    void setPitchBend(float bend01) {
        float semis = bend01 * 2.0f;
        for (int i = 0; i < NUM_VOICES; i++) voices[i].setPitchBend(semis);
    }

    uint8_t getVoiceActivity(bool* mask) const {
        uint8_t count = 0;
        for (int i = 0; i < NUM_VOICES; i++) {
            mask[i] = voices[i].isActive();
            if (mask[i]) count++;
        }
        return count;
    }

private:
    Voice     voices[MAX_VOICES];
    VoiceMode voiceMode    = VOICE_MODE_POLY;
    float     unisonDetune = 0.0f;   // max spread in cents
    uint8_t   unisonNote   = 255;    // currently held unison note (255 = none)

    uint32_t voiceAge[MAX_VOICES] = {0};
    uint32_t ageCounter = 0;

    uint8_t findFreeVoice();

    // Spread all NUM_VOICES voices evenly across [-unisonDetune, +unisonDetune] cents
    void applyUnisonDetune() {
        for (int i = 0; i < NUM_VOICES; i++) {
            float norm = (NUM_VOICES > 1)
                ? (i - (NUM_VOICES - 1) * 0.5f) / ((NUM_VOICES - 1) * 0.5f)
                : 0.0f;
            voices[i].setUnisonDetune(norm * unisonDetune);
        }
    }
};
