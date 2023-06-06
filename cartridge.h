#ifndef CARTRIDGE_H
#define CARTRIDGE_H

#include <fstream>
#include <iterator>
#include <vector>
#include "6507.h"
#include<iostream>

extern bool extraRAM128;
extern bool extraRAM256;
extern std::vector<uint8_t> ROMdata;
extern uint8_t* currentROM;
extern uint16_t ROMsize;

void loadCartridge(const char* ROMpath);
void getCartridgeData(std::vector<uint8_t>* cart);
void cartWrite(uint16_t loc, uint8_t val);
uint8_t cartRead(uint16_t loc);
void bankswitch(uint16_t loc);

#endif