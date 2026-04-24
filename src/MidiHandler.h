#pragma once
#include <Arduino.h>
#include <MIDI.h>
#include "Pins.h"

class Synth;

class MidiHandler {
public:
    void begin(Synth* s);
    void poll();

    void handleNoteOn(uint8_t ch, uint8_t note, uint8_t vel);
    void handleNoteOff(uint8_t ch, uint8_t note, uint8_t vel);
    void handleCC(uint8_t ch, uint8_t cc, uint8_t val);
    void handlePitchBend(uint8_t ch, int value);

private:
    Synth* synth = nullptr;
};
