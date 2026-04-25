#include "VoiceManager.h"
#include <math.h>

uint8_t VoiceManager::findFreeVoice() {
    for (int i = 0; i < NUM_VOICES; i++)
        if (!voices[i].isActive()) return i;
    // Steal oldest
    uint8_t  oldest = 0;
    uint32_t minAge = voiceAge[0];
    for (int i = 1; i < NUM_VOICES; i++) {
        if (voiceAge[i] < minAge) { minAge = voiceAge[i]; oldest = i; }
    }
    return oldest;
}

void VoiceManager::noteOn(uint8_t note, uint8_t vel) {
    if (voiceMode == VOICE_MODE_UNISON) {
        if (unisonNote != 255 && unisonNote != note) {
            for (int i = 0; i < NUM_VOICES; i++) voices[i].noteOff();
        }
        unisonNote = note;
        applyUnisonDetune();
        for (int i = 0; i < NUM_VOICES; i++) {
            voices[i].noteOn(note, vel);
            voiceAge[i] = ++ageCounter;
        }
    } else if (voiceMode == VOICE_MODE_MONO) {
        allNotesOff();
        voices[0].setUnisonDetune(0.0f);
        voices[0].noteOn(note, vel);
        voiceAge[0] = ++ageCounter;
    } else {
        uint8_t v = findFreeVoice();
        voices[v].setUnisonDetune(0.0f);
        voices[v].noteOn(note, vel);
        voiceAge[v] = ++ageCounter;
    }
}

void VoiceManager::noteOff(uint8_t note) {
    if (voiceMode == VOICE_MODE_UNISON) {
        if (unisonNote == note) {
            for (int i = 0; i < NUM_VOICES; i++) voices[i].noteOff();
            unisonNote = 255;
        }
    } else {
        for (int i = 0; i < NUM_VOICES; i++)
            if (voices[i].isActive() && voices[i].getNote() == note)
                voices[i].noteOff();
    }
}

void VoiceManager::allNotesOff() {
    for (int i = 0; i < MAX_VOICES; i++) voices[i].noteOff();
    unisonNote = 255;
}

void VoiceManager::renderBlock(float* out, uint32_t n) {
    for (uint32_t i = 0; i < n; i++) out[i] = 0.0f;
    for (int i = 0; i < NUM_VOICES; i++)
        voices[i].renderBlock(out, n);

    float activeCount = 0.0f;
    for (int i = 0; i < NUM_VOICES; i++)
        if (voices[i].isActive()) activeCount += 1.0f;
    if (activeCount < 1.0f) return;

    float scale = (voiceMode == VOICE_MODE_UNISON)
        ? 1.0f / activeCount
        : 1.0f / sqrtf(activeCount);
    for (uint32_t i = 0; i < n; i++) out[i] *= scale;
}
