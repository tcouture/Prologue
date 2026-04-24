#include "VoiceManager.h"
#include <math.h>

uint8_t VoiceManager::findFreeVoice() {
    for (int i = 0; i < NUM_VOICES; i++)
        if (!voices[i].isActive()) return i;
    uint8_t oldest = 0;
    uint32_t minAge = voiceAge[0];
    for (int i = 1; i < NUM_VOICES; i++) {
        if (voiceAge[i] < minAge) { minAge = voiceAge[i]; oldest = i; }
    }
    return oldest;
}

void VoiceManager::noteOn(uint8_t note, uint8_t vel) {
    uint8_t v = findFreeVoice();
    voices[v].noteOn(note, vel);
    voiceAge[v] = ++ageCounter;
}

void VoiceManager::noteOff(uint8_t note) {
    for (int i = 0; i < NUM_VOICES; i++)
        if (voices[i].isActive() && voices[i].getNote() == note)
            voices[i].noteOff();
}

void VoiceManager::allNotesOff() {
    for (int i = 0; i < MAX_VOICES; i++) voices[i].noteOff();
}

void VoiceManager::renderBlock(float* out, uint32_t n) {
    for (uint32_t i = 0; i < n; i++) out[i] = 0.0f;
    for (int i = 0; i < NUM_VOICES; i++)
        voices[i].renderBlock(out, n);
    float scale = 1.0f / sqrtf((float)NUM_VOICES);
    for (uint32_t i = 0; i < n; i++) out[i] *= scale;
}

void VoiceManager::setPitchBend(float bend01) {
    float semis = bend01 * 2.0f;
    for (int i = 0; i < NUM_VOICES; i++) voices[i].setPitchBend(semis);
}
