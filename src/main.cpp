#include <Arduino.h>
#include "AudioGraph.h"
#include "Synth.h"
#include "MidiHandler.h"
#include "PatchStorage.h"
#include "UI/Display.h"
#include "UI/UI.h"

// Uncomment to print raw touch coordinates for calibration
// #define CALIBRATE_TOUCH

Synth          g_synth;
MidiHandler    g_midi;
PatchStorage   g_storage;
Display        g_display;
UI             g_ui;

void setup() {
    Serial.begin(115200);
    delay(300);

    audioInit();
    g_synth.init();
    g_midi.begin(&g_synth);
    g_storage.begin();
    g_display.begin();
    g_ui.begin(&g_display, &g_synth, &g_storage);

    Serial.println("Prologue emulator online.");
}

void loop() {
    g_midi.poll();
    g_ui.poll();

#ifdef CALIBRATE_TOUCH
    static uint32_t lastPrint = 0;
    if (g_display.touch().touched() && millis() - lastPrint > 100) {
        TS_Point p = g_display.touch().getPoint();
        Serial.printf("raw x=%d y=%d z=%d\n", p.x, p.y, p.z);
        lastPrint = millis();
    }
#endif
}
