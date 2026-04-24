# Usage Guide

## Boot

On power-up you'll see the OSC page with 8 knobs and "INIT PATCH" in amber.

## Navigation

- Top tabs: OSC, FILTER, ENV, LFO, FX, GLOBAL, PATCH
- Tap a knob to select (amber), drag up/down to adjust.
- PATCH page: tap a slot to load, tap SAVE CUR to save current edit to
  slot 0 of the current bank, use < PAGE / PAGE > to browse 16 banks.

## MIDI Control

Any MIDI controller works over USB or DIN. Default CC map:

| CC | Parameter |
|----|-----------|
| 5  | Portamento |
| 7  | Master volume |
| 16 | VCO1 shape |
| 17 | VCO2 shape |
| 18 | VCO2 pitch |
| 20/21/22 | Mixer VCO1/VCO2/Multi |
| 43 | Cutoff |
| 44 | Resonance |
| 45 | Drive |
| 73 | Amp attack |
| 75 | Amp decay |
| 30 | Amp sustain |
| 72 | Amp release |
| 24-28 | Mod EG ADSR + amount |
| 76 | LFO rate |
| 77 | LFO depth |

Pitch bend is +/-2 semitones.

## Patches

Patches save to /patches/pNNN.bin on the SD card, 128 slots total.
