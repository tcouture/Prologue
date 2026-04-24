#pragma once

class Filter {
public:
    void init(float sr) { sampleRate = sr; updateCoeffs(); }
    void setCutoff(float hz)     { cutoff = hz; updateCoeffs(); }
    void setResonance(float q)   { res = q; updateCoeffs(); }
    void setDrive(float d)       { drive = 1.0f + d * 3.0f; }
    float processLP(float in);

private:
    void updateCoeffs();
    float sampleRate = 44100.0f;
    float cutoff = 1000.0f, res = 0.5f, drive = 1.0f;
    float g = 0.0f, k = 0.0f, a1 = 0.0f;
    float ic1eq = 0.0f, ic2eq = 0.0f;
};
