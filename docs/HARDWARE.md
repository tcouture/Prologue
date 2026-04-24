# Hardware Wiring

## Required Components

- Teensy 4.1
- Teensy Audio Adapter Board (Rev D or newer)
- ILI9341 2.8"/3.2" SPI TFT display (320x240)
- XPT2046 resistive touchscreen (usually integrated with the ILI9341 module)
- MIDI DIN-5 input circuit (6N138 optoisolator + protection resistors)
- microSD card (inserted into audio shield's SD slot)
- 10k ohm pullup resistor for display RESET (to 3.3V)

## Pin Assignments

All devices share SPI0 (MOSI=11, MISO=12, SCK=13).

| Function | Teensy Pin | Notes |
|---|---|---|
| I2S audio | 7, 8, 20, 21, 23 | Audio shield (automatic) |
| I2C audio | 18, 19 | Audio shield (automatic) |
| SD card CS | 10 | Audio shield built-in slot |
| Display CS | 40 | ILI9341 |
| Display DC | 9 | ILI9341 |
| Display RESET | - | Tied high via 10k pullup |
| Touch CS | 41 | XPT2046 |
| Touch IRQ | 2 | XPT2046 T_IRQ |
| DIN MIDI RX | 25 | Serial6 RX (via 6N138) |
| DIN MIDI TX | 24 | Serial6 TX (optional MIDI-out) |

## MIDI-IN Circuit (standard)

MIDI DIN pin 4 -> 220 ohm -> 6N138 pin 2 (anode)
MIDI DIN pin 5 -> 6N138 pin 3 (cathode)
6N138 pin 5 -> GND
6N138 pin 8 -> +3.3V (via 10k to pin 6)
6N138 pin 6 -> Teensy pin 25 (RX6), with 10k pullup to +3.3V

A 1N914 / 1N4148 flyback diode across pins 2-3 of the 6N138 is standard.

## Display Connections

Most ILI9341 breakout modules have combined display+touch headers. Confirm your
module exposes T_IRQ (sometimes labeled PEN or IRQ). If your module lacks
T_IRQ, set TOUCH_IRQ to 255 in Pins.h and the code will fall back to polling.
