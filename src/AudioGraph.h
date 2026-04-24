#pragma once
#include <Audio.h>
#include "SynthAudio.h"

extern SynthAudio             synthAudio;
extern AudioOutputI2S         i2sOut;
extern AudioControlSGTL5000   sgtl;

void audioInit();
