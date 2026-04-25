#include "Voice.h"
#include <math.h>

static inline float midiToHz(float note) {
    return 440.0f * powf(2.0f, (note - 69.0f) / 12.0f);
}

void Voice::init(float sr) {
    sampleRate = sr;
    vco1.init(sr);
    vco2.init(sr);
    filter.init(sr);
    ampEG.init(sr);
    modEG.init(sr);
    lfo.init(sr);

    cutoffSmooth.init(sr,    5.0f);
    pitchBendSmooth.init(sr, 8.0f);
    mix1Smooth.init(sr,     15.0f);
    mix2Smooth.init(sr,     15.0f);
    mixNoisSmooth.init(sr,  15.0f);
}

void Voice::noteOn(uint8_t note, uint8_t vel) {
    targetNote = note;
    velocity   = vel / 127.0f;

    float newFreq = midiToHz(note + vco1OctShift * 12);
    targetFreqHz  = newFreq;

    if (portaTimeSec <= 0.0005f || !ampEG.isActive()) {
        baseFreqHz = newFreq;
        portaCoeff = 1.0f;
    } else {
        portaCoeff = 1.0f - expf(-1.0f / (portaTimeSec * sampleRate));
    }

    currentNote = note;
    ampEG.noteOn();
    modEG.noteOn();
}

void Voice::noteOff() {
    ampEG.noteOff();
    modEG.noteOff();
}

void Voice::setParams(const float* p) {
    vco1.setShape((WaveShape)(int)(p[P_VCO1_WAVE] * 2.99f));
    vco1.setShapeMod(p[P_VCO1_SHAPE]);
    vco2.setShape((WaveShape)(int)(p[P_VCO2_WAVE] * 2.99f));
    vco2.setShapeMod(p[P_VCO2_SHAPE]);

    vco1OctShift = (int)(p[P_VCO1_OCT] * 4.99f) - 2;
    vco2OctShift = (int)(p[P_VCO2_OCT] * 4.99f) - 2;

    float cents     = (p[P_VCO2_PITCH] - 0.5f) * 200.0f;
    vco2DetuneRatio = powf(2.0f, cents / 1200.0f);

    mix1Smooth.setTarget(p[P_MIX_VCO1]);
    mix2Smooth.setTarget(p[P_MIX_VCO2]);
    mixNoisSmooth.setTarget(p[P_MIX_MULTI]);

    baseCutoff = 20.0f * powf(1000.0f, p[P_FILT_CUTOFF]);
    cutoffSmooth.setTarget(baseCutoff);
    filter.setResonance(p[P_FILT_RESO] * 0.97f);
    filter.setDrive(p[P_FILT_DRIVE]);
    keyTrack = p[P_FILT_KEYTRK];

    auto scaleTime = [](float x){ return 0.001f * powf(10000.0f, x); };
    ampEG.setADSR(scaleTime(p[P_AEG_A]), scaleTime(p[P_AEG_D]),
                  p[P_AEG_S],             scaleTime(p[P_AEG_R]));
    modEG.setADSR(scaleTime(p[P_MEG_A]), scaleTime(p[P_MEG_D]),
                  p[P_MEG_S],             scaleTime(p[P_MEG_R]));
    modEGAmount = (p[P_MEG_AMT] - 0.5f) * 2.0f;

    lfoShape = (LFOShape)(int)(p[P_LFO_WAVE] * 4.99f);
    lfo.setShape(lfoShape);
    lfo.setRate(0.05f * powf(400.0f, p[P_LFO_RATE]));
    lfoDepth = p[P_LFO_DEPTH];
    lfoDest  = (uint8_t)(p[P_LFO_DEST] * 3.99f);

    fmDepth = p[P_VCO2_CROSS_MOD] * 4.0f;

    if (p[P_PORTAMENTO] < 0.01f)
        setPortamentoTime(0.0f);
    else
        setPortamentoTime(0.001f * powf(2000.0f, p[P_PORTAMENTO]));

    // Note: unisonDetuneCents is set externally by VoiceManager, not from params,
    // so we do NOT touch it here.
}

void Voice::renderBlock(float* out, uint32_t n) {
    if (!ampEG.isActive()) return;

    float keyOffset = powf(2.0f, ((float)currentNote - 60.0f) / 12.0f * keyTrack);

    for (uint32_t i = 0; i < n; i++) {
        // --- Portamento ---
        if (portaCoeff < 1.0f) {
            baseFreqHz += portaCoeff * (targetFreqHz - baseFreqHz);
            if (fabsf(targetFreqHz - baseFreqHz) < 0.1f) baseFreqHz = targetFreqHz;
        }

        // --- Pitch bend + unison detune + LFO ---
        pitchBendSmooth.setTarget(pitchBendSemis);
        pitchBendSmooth.process();
        float lfoVal = lfo.process() * lfoDepth;

        float pitchModSemis = (lfoDest == LFO_DEST_PITCH) ? lfoVal * 2.0f : 0.0f;
        // unisonDetuneCents is in cents; convert to semitones for the combined multiplier
        float totalSemis = pitchBendSmooth.value() + pitchModSemis
                         + unisonDetuneCents * 0.01f;
        float freqMul = powf(2.0f, totalSemis / 12.0f);

        // --- VCO frequencies ---
        float vco1Freq = baseFreqHz * freqMul;
        float v2base   = baseFreqHz * powf(2.0f, (float)(vco2OctShift - vco1OctShift));
        float vco2Freq = v2base * vco2DetuneRatio * freqMul;

        vco1.setFrequency(vco1Freq);
        vco2.setFrequency(vco2Freq);

        // --- Shape LFO ---
        if (lfoDest == LFO_DEST_SHAPE) {
            float s = 0.5f + lfoVal * 0.5f;
            if (s < 0.02f) s = 0.02f;
            if (s > 0.98f) s = 0.98f;
            vco1.setShapeMod(s);
            vco2.setShapeMod(s);
        }

        // --- VCO2 first (FM modulator) ---
        float o2raw = vco2.process();

        // --- FM: VCO2 -> VCO1 phase modulation ---
        if (fmDepth > 0.0f) {
            float phOffset = o2raw * fmDepth * (vco2Freq / vco1Freq);
            if (phOffset >= 1.0f)  phOffset -= (int) phOffset + 1;
            if (phOffset <  0.0f)  phOffset += (int)(-phOffset) + 1;
            vco1.setPhaseOffset(phOffset);
        }

        float o1    = vco1.process()  * mix1Smooth.process();
        float o2    = o2raw           * mix2Smooth.process();
        float noise = nextNoise()     * mixNoisSmooth.process();
        float mix   = (o1 + o2 + noise) * 0.5f;

        // --- Filter ---
        float modEnv = modEG.process();
        float cutMod = cutoffSmooth.process() * keyOffset
                     * powf(2.0f, modEnv * modEGAmount * 4.0f);
        if (lfoDest == LFO_DEST_CUTOFF) cutMod *= powf(2.0f, lfoVal * 3.0f);
        if (cutMod > 18000.0f) cutMod = 18000.0f;
        if (cutMod <    20.0f) cutMod =    20.0f;
        filter.setCutoff(cutMod);

        float s = filter.processLP(mix);

        // --- Amp ---
        float amp = ampEG.process() * velocity;
        if (lfoDest == LFO_DEST_AMP) amp *= (1.0f + lfoVal * 0.5f);

        out[i] += s * amp;
    }
}
