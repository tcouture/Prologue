#pragma once
#include <Audio.h>
#include "VoiceManager.h"
#include "Chorus.h"
#include "Delay.h"
#include "Reverb.h"
#include "ParamSmoother.h"
#include "Params.h"

class SynthAudio : public AudioStream {
public:
    SynthAudio() : AudioStream(0, nullptr) {
        voices.init(AUDIO_SAMPLE_RATE_EXACT);
        chorus.init(AUDIO_SAMPLE_RATE_EXACT);
        delay.init(AUDIO_SAMPLE_RATE_EXACT);
        reverb.init(AUDIO_SAMPLE_RATE_EXACT);
        masterVolSmooth.init(AUDIO_SAMPLE_RATE_EXACT, 20.0f);
        masterVolSmooth.snap(0.7f);
    }

    VoiceManager& getVoices() { return voices; }
    void setMasterVolume(float v) { masterVolSmooth.setTarget(v); }

    void setFxParams(const float* p) {
        chorus.setDepth(p[P_FX_MOD_DEPTH]);
        chorus.setRate(0.5f + p[P_FX_MOD_TYPE] * 3.0f);
        delay.setTime  (0.05f + p[P_FX_DELAY_TIME] * 0.7f);
        delay.setFeedback(p[P_FX_DELAY_FB]);
        delay.setMix(p[P_FX_DELAY_MIX]);
        reverb.setSize(p[P_FX_REVERB_SIZE]);
        reverb.setMix(p[P_FX_REVERB_MIX]);
    }

    void update() override {
        audio_block_t *blockL = allocate();
        audio_block_t *blockR = allocate();
        if (!blockL || !blockR) {
            if (blockL) release(blockL);
            if (blockR) release(blockR);
            return;
        }

        float bufL[AUDIO_BLOCK_SAMPLES];
        float bufR[AUDIO_BLOCK_SAMPLES];
        for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) bufL[i] = bufR[i] = 0.0f;

        voices.renderBlock(bufL, AUDIO_BLOCK_SAMPLES);
        for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) bufR[i] = bufL[i];

        chorus.process(bufL, bufR, AUDIO_BLOCK_SAMPLES);
        delay.process (bufL, bufR, AUDIO_BLOCK_SAMPLES);
        reverb.process(bufL, bufR, AUDIO_BLOCK_SAMPLES);

        for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
            float mv = masterVolSmooth.process();
            float sl = bufL[i] * mv;
            float sr = bufR[i] * mv;
            if (sl > 1.0f) sl = 1.0f; 
            if (sl < -1.0f) sl = -1.0f;
            if (sr > 1.0f) sr = 1.0f; 
            if (sr < -1.0f) sr = -1.0f;
            blockL->data[i] = (int16_t)(sl * 32000.0f);
            blockR->data[i] = (int16_t)(sr * 32000.0f);
        }

        transmit(blockL, 0);
        transmit(blockR, 1);
        release(blockL);
        release(blockR);
    }

private:
    VoiceManager voices;
    Chorus chorus;
    Delay  delay;
    Reverb reverb;
    ParamSmoother masterVolSmooth;
};
