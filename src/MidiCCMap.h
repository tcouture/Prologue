#pragma once
#include "Params.h"

inline uint8_t ccToParam(uint8_t cc) {
    switch (cc) {
        case 16: return P_VCO1_SHAPE;
        case 17: return P_VCO2_SHAPE;
        case 18: return P_VCO2_PITCH;
        case 19: return P_VCO2_CROSS_MOD;   // FM DEPTH
        case 20: return P_MIX_VCO1;
        case 21: return P_MIX_VCO2;
        case 22: return P_MIX_MULTI;         // NOISE LEVEL
        case 43: return P_FILT_CUTOFF;
        case 44: return P_FILT_RESO;
        case 45: return P_FILT_DRIVE;
        case 73: return P_AEG_A;
        case 75: return P_AEG_D;
        case 30: return P_AEG_S;
        case 72: return P_AEG_R;
        case 24: return P_MEG_A;
        case 25: return P_MEG_D;
        case 26: return P_MEG_S;
        case 27: return P_MEG_R;
        case 28: return P_MEG_AMT;
        case 76: return P_LFO_RATE;
        case 77: return P_LFO_DEPTH;
        case  5: return P_PORTAMENTO;
        case  7: return P_MASTER_VOL;
        default: return PARAM_COUNT;
    }
}
