#include "MidiHandler.h"
#include "Synth.h"

MIDI_CREATE_INSTANCE(HardwareSerial, MIDI_SERIAL, dinMidi);

static MidiHandler* g_handler = nullptr;

static void onDinNoteOn(byte ch, byte n, byte v) {
    if (v == 0) g_handler->handleNoteOff(ch, n, v);
    else        g_handler->handleNoteOn(ch, n, v);
}
static void onDinNoteOff(byte ch, byte n, byte v) { g_handler->handleNoteOff(ch,n,v); }
static void onDinCC(byte ch, byte cc, byte val)   { g_handler->handleCC(ch,cc,val); }
static void onDinBend(byte ch, int val)           { g_handler->handlePitchBend(ch,val); }

void MidiHandler::begin(Synth* s) {
    synth = s;
    g_handler = this;

    dinMidi.begin(MIDI_CHANNEL_OMNI);
    dinMidi.setHandleNoteOn(onDinNoteOn);
    dinMidi.setHandleNoteOff(onDinNoteOff);
    dinMidi.setHandleControlChange(onDinCC);
    dinMidi.setHandlePitchBend(onDinBend);

    usbMIDI.setHandleNoteOn([](uint8_t ch, uint8_t n, uint8_t v){
        if (v == 0) g_handler->handleNoteOff(ch,n,v);
        else        g_handler->handleNoteOn(ch,n,v);
    });
    usbMIDI.setHandleNoteOff([](uint8_t ch, uint8_t n, uint8_t v){
        g_handler->handleNoteOff(ch,n,v);
    });
    usbMIDI.setHandleControlChange([](uint8_t ch, uint8_t cc, uint8_t v){
        g_handler->handleCC(ch,cc,v);
    });
    usbMIDI.setHandlePitchChange([](uint8_t ch, int v){
        g_handler->handlePitchBend(ch,v);
    });
}

void MidiHandler::poll() {
    dinMidi.read();
    usbMIDI.read();
}

void MidiHandler::handleNoteOn(uint8_t, uint8_t note, uint8_t vel) {
    synth->noteOn(note, vel);
}
void MidiHandler::handleNoteOff(uint8_t, uint8_t note, uint8_t) {
    synth->noteOff(note);
}
void MidiHandler::handleCC(uint8_t, uint8_t cc, uint8_t val) {
    synth->handleCC(cc, val);
}
void MidiHandler::handlePitchBend(uint8_t, int value) {
    synth->setPitchBend(value);
}
