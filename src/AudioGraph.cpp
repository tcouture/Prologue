#include "AudioGraph.h"

SynthAudio           synthAudio;
AudioOutputI2S       i2sOut;
AudioControlSGTL5000 sgtl;
AudioConnection      patchL(synthAudio, 0, i2sOut, 0);
AudioConnection      patchR(synthAudio, 1, i2sOut, 1);

void audioInit() {
    AudioMemory(32);
    sgtl.enable();
    sgtl.volume(0.6f);
    sgtl.lineOutLevel(13);
}
