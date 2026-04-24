#pragma once

// Shared SPI0 bus (display + touch + SD)
#define SPI_MOSI   11
#define SPI_MISO   12
#define SPI_SCK    13

// Display (ILI9341)
#define TFT_CS     40
#define TFT_DC     9
#define TFT_RST    255   // 255 = not connected; external 10k pullup

// Touch (XPT2046)
#define TOUCH_CS   41
#define TOUCH_IRQ  2

// SD card (audio shield)
#define SD_CS      10

// DIN MIDI on Serial6 (pins 24 TX / 25 RX)
#define MIDI_SERIAL Serial6
