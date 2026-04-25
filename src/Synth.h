#pragma once
#include <string.h>
#include "Params.h"
#include "AudioGraph.h"
#include "MidiCCMap.h"

class Synth {
public:
    void init() {
        for (int i = 0; i < PARAM_COUNT; i++) patch.params[i] = 0.5f;
        applyDefaults();
        pushPatchToVoices();
    }

    void noteOn(uint8_t n, uint8_t v)  { synthAudio.getVoices().noteOn(n, v); }
    void noteOff(uint8_t n)            { synthAudio.getVoices().noteOff(n); }

    void handleCC(uint8_t cc, uint8_t val) {
        uint8_t p = ccToParam(cc);
        if (p < PARAM_COUNT) setParam(p, val / 127.0f);
    }

    void setPitchBend(int value) {
        float bend = value / 8192.0f;
        synthAudio.getVoices().setPitchBend(bend);
    }

    void setParam(uint8_t id, float value01) {
        if (id >= PARAM_COUNT) return;
        patch.params[id] = value01;
        synthAudio.getVoices().setParams(patch.params);
        synthAudio.setFxParams(patch.params);
        if (id == P_MASTER_VOL) synthAudio.setMasterVolume(value01);
        paramChanged = true;
    }

    float  getParam(uint8_t id) const { return patch.params[id]; }
    Patch& getPatch()                 { return patch; }

    void loadPatch(const Patch& newPatch) {
        synthAudio.getVoices().allNotesOff();
        patch = newPatch;
        pushPatchToVoices();
    }

    void pushPatchToVoices() {
        synthAudio.getVoices().setParams(patch.params);
        synthAudio.setFxParams(patch.params);
        synthAudio.setMasterVolume(patch.params[P_MASTER_VOL]);
        paramChanged = true;
    }

    bool consumeParamChanged() { bool c = paramChanged; paramChanged = false; return c; }

    uint8_t getVoiceActivity(bool* mask) const {
        return synthAudio.getVoices().getVoiceActivity(mask);
    }

private:
    Patch patch;
    bool  paramChanged = false;

    void applyDefaults() {
        patch.params[P_VCO1_WAVE]      = 0.0f;
        patch.params[P_VCO1_OCT]       = 0.5f;
        patch.params[P_VCO2_WAVE]      = 0.0f;
        patch.params[P_VCO2_OCT]       = 0.5f;
        patch.params[P_VCO2_PITCH]     = 0.5f;
        patch.params[P_MIX_VCO1]       = 0.8f;
        patch.params[P_MIX_VCO2]       = 0.0f;
        patch.params[P_MIX_MULTI]      = 0.0f;
        patch.params[P_FILT_CUTOFF]    = 0.7f;
        patch.params[P_FILT_RESO]      = 0.2f;
        patch.params[P_FILT_DRIVE]     = 0.0f;
        patch.params[P_FILT_KEYTRK]    = 0.0f;
        patch.params[P_AEG_A]          = 0.1f;
        patch.params[P_AEG_D]          = 0.4f;
        patch.params[P_AEG_S]          = 0.7f;
        patch.params[P_AEG_R]          = 0.3f;
        patch.params[P_MEG_A]          = 0.0f;
        patch.params[P_MEG_D]          = 0.4f;
        patch.params[P_MEG_S]          = 0.0f;
        patch.params[P_MEG_R]          = 0.3f;
        patch.params[P_MEG_AMT]        = 0.5f;
        patch.params[P_LFO_WAVE]       = 0.0f;
        patch.params[P_LFO_RATE]       = 0.3f;
        patch.params[P_LFO_DEPTH]      = 0.0f;
        patch.params[P_LFO_DEST]       = 0.0f;
        patch.params[P_VCO2_CROSS_MOD] = 0.0f;
        patch.params[P_FX_MOD_DEPTH]   = 0.0f;
        patch.params[P_FX_DELAY_MIX]   = 0.0f;
        patch.params[P_FX_REVERB_MIX]  = 0.0f;
        patch.params[P_MASTER_VOL]     = 0.7f;
        patch.params[P_PORTAMENTO]     = 0.0f;
        patch.params[P_VOICE_MODE]     = 0.0f;
        patch.params[P_UNISON_DETUNE]  = 0.2f;  // default: 10 cents spread (nice out of the box)
        strncpy(patch.name, "INIT", 16);
        patch.name[16] = 0;
    }
};
