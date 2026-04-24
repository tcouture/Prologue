# Architecture

## Signal Flow

```
MIDI (USB + DIN)
  -> Synth
  -> Patch params -> VoiceManager -> 8x Voice
  -> each Voice: VCO1 + VCO2 -> Filter -> Amp (with LFO + Mod EG)
  -> Voice sum (mono)
  -> Chorus -> Delay -> Reverb
  -> Master Volume -> I2S Out
```

```
MIDI (USB + DIN)
      │
      ▼
   Synth ──► Patch params ──► VoiceManager ──► 8× Voice
                                                  │
                                                  ▼
                                     Voice = VCO1 + VCO2 → Filter → Amp
                                                  │ (with LFO + Mod EG)
                                                  ▼
                                          Voice sum (mono)
                                                  │
                                                  ▼
                                   Chorus ──► Delay ──► Reverb
                                                  │
                                                  ▼
                                          Master Vol ──► I2S Out
```

## Threading Model

- Audio thread: SynthAudio::update() fires every 128 samples (~2.9 ms at
  44.1 kHz). Runs in the Teensy Audio Library's interrupt context.
- Main loop: handles MIDI parsing, touch input, display redraws. Touches
  the shared Patch only through atomic float writes.

## Parameter Smoothing

Every click-sensitive parameter (cutoff, mix, pitch bend, master volume)
passes through a ParamSmoother - a one-pole lowpass with ~5-20 ms time
constant. Eliminates zipper noise and patch-load clicks.

## Voice Allocation

VoiceManager::findFreeVoice():
1. Prefer any idle voice.
2. Otherwise, steal oldest by age counter.

## UI Model

- 7 pages: OSC, FILTER, ENV, LFO, FX, GLOBAL, PATCH
- Each knob page exposes 8 parameter knobs in a 4x2 grid
- Tap-to-select + drag-to-adjust (200 px drag = full sweep)
- Shared Patch is single source of truth; touch, MIDI CC, and patch
  load all funnel through Synth::setParam().

## SPI Bus Sharing

All SPI devices (display, touch, SD) share SPI0 with independent CS pins.
All libraries use SPI.beginTransaction() wrapping, so sharing is safe.
The audio thread never touches SPI (I2S only).
