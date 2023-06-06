#include "cartridge.h"

bool extraRAM128 = false;
bool extraRAM256 = false;

std::vector<uint8_t> ROMdata;
uint8_t* currentROM;   // Used to switch banks without actually moving data
uint16_t ROMsize;

void loadCartridge(const char* ROMpath) {
    std::ifstream ROMfile(ROMpath, std::ios::binary);
    ROMdata = std::vector<uint8_t>(std::istreambuf_iterator<char>(ROMfile), {});

    // I'm assuming for 8K carts the first 4k is loaded into memory first
    currentROM = &ROMdata[0];
    getCartridgeData(&ROMdata);

    // Cartridges of 2KBytes are mirrored twice in the 4K cartridge area.
    // Is more fitting in the bankswitch func defenition-wise, but not implementation-wise imo
    
    return;
}

void getCartridgeData(std::vector<uint8_t>* cart) {
    ROMsize = cart->size();

    // The first 256 bytes (or 512 bytes) should be 00h or FFh filled, when using extra RAM
    // (Omegarace and Digdug use garbage filled area though, so will not work)
    uint8_t fillByte = (*cart)[0];
    for (uint16_t i = 0; i <= 0x200; i++) {
        if ((*cart)[i] != fillByte) {
            break;
        }
        if (i == 0x200) {   // CBS  RAM Plus (RAM+)
            extraRAM256 = true;
            extraRAM128 = false;
        }
        if (i == 0xFF) {    // Super Chip
            extraRAM128 = true;
        }
    }

    if (DEBUG) {
        std::cout << "Size:             " << (int)ROMsize << " bytes" << std::endl;
        std::cout << "Expansion RAM:    " << (extraRAM128 ? "128" : (extraRAM256 ? "256" : "0")) << " bytes" << std::endl;
    }
}

void cartWrite(uint16_t loc, uint8_t val) {
    // 2K roms are repeated
    if (ROMsize == 0x800 && loc > 0x1800) {
        loc -= 0x800;
    }

    bankswitch(loc);
    currentROM[loc-0x1000] = val;
}

uint8_t cartRead(uint16_t loc) {
    // 2K roms are repeated
    if (ROMsize == 0x800 && loc > 0x1800) {
        loc -= 0x800;
    }

    bankswitch(loc);

    if (extraRAM128 && loc <= 0x10ff && loc >= 0x1080) {
        loc -= 0x80;
        return ROMdata[loc];
    }
    else if (extraRAM256 && loc <= 0x11ff && loc >= 0x1100) {
        loc -= 0x100;
        return ROMdata[loc];
    }

    return currentROM[loc-0x1000];
}

void bankswitch(uint16_t loc) {
    if ((loc < 0x1FF6 && loc > 0x1FF9) || (ROMsize != 0x2000 && ROMsize != 0x4000)) {
        return;
    }
    uint8_t* prevBank = currentROM;
    if (ROMsize == 0x2000 && loc == 0x1FF8) {   // 8K standard bankswitching #0
        currentROM = &ROMdata[0];
    }
    if (ROMsize == 0x2000 && loc == 0x1FF9) {   // 8K standard bankswitching #1
        currentROM = &ROMdata[0x1000];
    }

    if (ROMsize == 0x4000 && loc == 0x1FF6) {   // 16K standard bankswitching #0
        currentROM = &ROMdata[0];
    }
    if (ROMsize == 0x4000 && loc == 0x1FF7) {   // 16K standard bankswitching #1
        currentROM = &ROMdata[0x1000];
    }
    if (ROMsize == 0x4000 && loc == 0x1FF8) {   // 16K standard bankswitching #2
        currentROM = &ROMdata[0x2000];
    }
    if (ROMsize == 0x4000 && loc == 0x1FF9) {   // 16K standard bankswitching #3
        currentROM = &ROMdata[0x3000];
    }
    // Not implementing 32K cause only like 1 game actually uses it

    if (DEBUG && currentROM != prevBank) {
        std::cout << "switched banks to: #" << (int)((currentROM - &ROMdata[0]) / 0x1000) << std::endl;
    }

}