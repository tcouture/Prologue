# Prologue Teensy Synth

A Korg Prologue-inspired polyphonic synthesizer for the Teensy 4.1, built with
PlatformIO and the Teensy Audio Library.

## Features

- 8-voice polyphony (compile-time extensible to 16)
- Dual VCO (saw / triangle / square) with shape modifier
- State-variable lowpass filter with drive and key tracking
- Amp + Mod ADSR envelopes
- Per-voice LFO with 5 shapes and 4 destinations
- Portamento, pitch bend (±2 semitones)
- Master-bus FX: chorus, ping-pong delay, Freeverb reverb
- USB MIDI + DIN MIDI input (merged)
- Touchscreen UI on ILI9341 + XPT2046
- Patch storage on audio-shield SD card (128 slots)
- Click-free parameter smoothing

## Hardware

See [docs/HARDWARE.md](docs/HARDWARE.md) for the full wiring guide.

## Building

See [docs/BUILD.md](docs/BUILD.md).

## Architecture

See [docs/ARCHITECTURE.md](docs/ARCHITECTURE.md) for DSP and UI design notes.

## Usage

See [docs/USAGE.md](docs/USAGE.md) for playing and patch-editing instructions.
