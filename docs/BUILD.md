# Building & Flashing

## Directory Structure

```
prologue-teensy/
├── .gitignore
├── platformio.ini
├── README.md
├── docs/
│   ├── ARCHITECTURE.md
│   ├── BUILD.md
│   ├── HARDWARE.md
│   └── USAGE.md
└── src/
    ├── main.cpp
    ├── Pins.h
    ├── Params.h
    ├── ParamSmoother.h
    ├── Oscillator.h / .cpp
    ├── Filter.h / .cpp
    ├── Envelope.h / .cpp
    ├── LFO.h
    ├── Voice.h / .cpp
    ├── VoiceManager.h / .cpp
    ├── Chorus.h / .cpp
    ├── Delay.h / .cpp
    ├── Reverb.h / .cpp
    ├── SynthAudio.h
    ├── AudioGraph.h / .cpp
    ├── MidiCCMap.h
    ├── Synth.h
    ├── MidiHandler.h / .cpp
    ├── PatchStorage.h
    └── UI/
        ├── Theme.h
        ├── Display.h / .cpp
        ├── UIWidgets.h / .cpp
        ├── UIPages.h / .cpp
        └── UI.h / .cpp
```


## Prerequisites

1. Visual Studio Code
2. PlatformIO IDE extension
3. Teensy USB driver (auto-installed on macOS/Linux; 

Windows needs the Teensyduino installer from pjrc.com)

## First Build
```
cd prologue-teensy
code .
````

In VS Code:
1. PlatformIO will auto-fetch dependencies on first open.
2. Click the PlatformIO Build button at bottom.
3. Click the Upload button to flash.

From CLI:
```
pio run              # build
pio run -t upload    # flash
pio device monitor   # serial log at 115200
````

## Touch Calibration

On first boot, uncomment `#define CALIBRATE_TOUCH` in `src/main.cpp` and
reflash. Watch the serial monitor while touching all four corners; note the
min/max raw X/Y values, update `_xMin/_xMax/_yMin/_yMax` in
`src/UI/Display.h`, recomment the define, and reflash.

## Memory & CPU

With 8 voices + full FX chain:
- RAM1 usage: ~90 KB
- RAM2 usage: ~50 KB
- CPU: ~30-40% at 44.1 kHz

Bumping NUM_VOICES in src/Params.h to 16 raises CPU to ~55-65%.

## Bumping to 16 Voices Later

Everything is ready. When you want to make the switch:

    Change NUM_VOICES in Params.h from 8 to 16.
    Rebuild.


CPU estimate at 16 voices with full FX at 44.1 kHz on Teensy 4.1: roughly 55–65%. Still comfortable. If you feel it getting tight, the first optimization is to run envelope/LFO/cutoff updates at block rate (every 128 samples) instead of per-sample — that alone gives a 15–20% headroom boost.