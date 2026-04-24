#pragma once
#include <SD.h>
#include <SPI.h>
#include "Params.h"
#include "Pins.h"

class PatchStorage {
public:
    bool begin() {
        if (!SD.begin(SD_CS)) {
            Serial.println("SD init failed");
            return false;
        }
        if (!SD.exists("/patches")) SD.mkdir("/patches");
        return true;
    }

    bool save(uint8_t slot, const Patch& p) {
        char path[32];
        snprintf(path, sizeof(path), "/patches/p%03u.bin", slot);
        if (SD.exists(path)) SD.remove(path);
        File f = SD.open(path, FILE_WRITE);
        if (!f) return false;
        f.write((const uint8_t*)&p, sizeof(Patch));
        f.close();
        return true;
    }

    bool load(uint8_t slot, Patch& p) {
        char path[32];
        snprintf(path, sizeof(path), "/patches/p%03u.bin", slot);
        File f = SD.open(path, FILE_READ);
        if (!f) return false;
        if (f.size() != sizeof(Patch)) { f.close(); return false; }
        f.read((uint8_t*)&p, sizeof(Patch));
        f.close();
        return true;
    }
};
