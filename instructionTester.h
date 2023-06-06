#ifndef INSTRUCTION_TESTER_H
#define INSTRUCTION_TESTER_H

#include "6507.h"
#include "cartridge.h"
#include <chrono>
#include <thread>
#include<string>
#include "debug.h"
#include <thread>
#include "json11.hpp"

extern json11::Json testROMS[0xFF];
extern std::vector<int> currentREGfails;
extern std::map<std::string, int> totalADDRfails;
extern std::map<std::string, int> totalADDRtests;

extern bool testIgnoreStatus;// -ignoreStatus
extern bool testIgnoreCycles;// -ignoreCycles
extern bool testIgnoreRegisters;// -ignoreRegisters
extern bool testIgnoreMemory;// -ignoreMemory
extern bool testNoBCD;// -noBCD


extern int testAmount;//Will default to 10000 if not set, can be set without flag, just number
extern int testOpcode;//Will default to all if not set, can be set without flag, just hex number (0xnn)

// 1 = passed, 0 = failed, -1 = illegal opcode
int doTest(int ROMindex, int testIndex);
void loadTestROM(int ROMindex);
int testInstructions();

#endif

