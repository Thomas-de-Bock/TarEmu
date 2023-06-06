#include "6507.h"
#include "cartridge.h"
#include <chrono>
#include <thread>
#include<string>
#include "debug.h"
#include <thread>
#include "instructionTester.h"
#include "EmuGraphics.h"
#include <thread>
#include<iostream>
#include "PIA.h"
#include "EmuAudio.h"
#include <filesystem>

bool is_number(const std::string& s) {
    std::string::const_iterator it = s.begin();
    while (it != s.end() && std::isdigit(*it)) ++it;
    return !s.empty() && it == s.end();
}
// Print state in same format as Stella, used for debugging
// Reccomended to use with output redirected to file
void dumpState() { 
    // Dump memory 00-ff
    for (uint16_t i = 0x80; i <= 0xFF; i++) {
        if (i % 0x10 == 0) {
            if (i != 0x80) {
                std::cout << std::endl;
            }
            std::cout << getHex((uint8_t)i) << ": ";
        }
        else if (i % 0x08 == 0) {
            std::cout << "- ";

        }
        std::cout << getHex(GETMEM8(i)) << " ";
    }
    // Dump registers
    std::cout << std::endl;
    std::cout << "   <PC>PC SP  A  X  Y  -  -    N  V  B  D  I  Z  C  -" << std::endl;
    std::cout << "XC: " << getHex((uint8_t)(PC & 0x00FF)) << " " << getHex((uint8_t)((PC & 0xFF00)>>8));
    std::cout << " " << getHex(S) << " " << getHex(A) << " " << getHex(X) << " " << getHex(Y) << " 00 00";
    // Flags
    std::cout << " - " << getHex((uint8_t)((P&0b10000000) >> 7)) << " " << getHex((uint8_t)((P & 0b01000000)>> 6));
    std::cout << " " << getHex((uint8_t)((P & 0b00010000) >> 4)) << " ";
    std::cout << getHex((uint8_t)((P & 0b00001000) >> 3)) << " " << getHex((uint8_t)((P & 0b00000100) >> 2)) << " ";
    std::cout << getHex((uint8_t)((P & 0b00000010)>>1)) << " " << getHex((uint8_t)(P & 0b00000001)) << " ";
    std::cout << "00 " << std::endl;

    std::cout << "   SWA - SWB  - IT  -  -  -   I0 I1 I2 I3 I4 I5 -  -" << std::endl;







}

int Atariloop() {
    bool endAtari = false;
    int deltaClockCPU = 0;
    int deltaClock = 0;
    uint8_t deltaCPUcycles = 0;
    if (DUMP) {
        dumpState();
    }

    tick6507();
    deltaCPUcycles = cycles;
    while (!endAtari) {
        if (!cycleButton && !stepLineButton && !stepFrameButton && DEBUG) {
            continue;
        }

        using namespace std::chrono_literals;
        using namespace std::chrono;

        auto startTime = std::chrono::steady_clock::now();
        
        tickTIA();
        deltaClock++;
        deltaClockCPU++;
        if (RDY && deltaClockCPU >= 3* deltaCPUcycles) {
            doBufferedTIA();

            uint8_t cyclesBefore = cycles;
            if (DUMP && steps <= 19912) {
                dumpState();
            }
            else if (DUMP) {
                return 0;
            }
            tick6507();
            deltaCPUcycles = cycles - cyclesBefore;
            deltaClockCPU = 0;
            tickAudio();
            if (cycleButton) {
                EmuGraphics::drawScreen();
                cycleButton = false;
            }
        }
        if (deltaClock == 3) {
            tickPIA();
            deltaClock = 0;
        }
        // 838.09511038551 ns per cycle, should probably just wait at end of frame for correct FPS
        while ((std::chrono::steady_clock::now() - startTime) < duration<double, std::nano>(838/3)) continue;
    }
    if (DEBUG) {
        endDebug();
    }
    EmuGraphics::closeScreen();
    return 0;
}

int CPUloop() {
    return 0;
}

// This whole function is so bad I have no idea what the standard or proper way to do this is
int main(int argc, char** argv) {
    bool isTest = false;            // --jsonTest      
    const char* ROMpath = nullptr;
    for (int i = 1; i < argc; i++) {
        if (std::string(argv[i]) == "--jsonTest") {
            isTest = true;
        }
        else if (std::string(argv[i]) == "-ignoreStatus") {
            testIgnoreStatus = true;
        }
        else if (std::string(argv[i]) == "-ignoreCycles") {
            testIgnoreCycles = true;
        } 
        else if (std::string(argv[i]) == "-ignoreRegisters") {
            testIgnoreRegisters = true;
        }
        else if (std::string(argv[i]) == "-ignoreMemory") {
            testIgnoreMemory = true;
        }
        else if (std::string(argv[i]) == "-noBCD") {
            testNoBCD = true;
        }
        else if (std::string(argv[i]) == "-noCrop") {
            EmuGraphics::cropScreen = false;
        }
        else if (std::string(argv[i]) == "-rom") {
            ROMpath = argv[++i];
        }
        else if (std::string(argv[i]) == "-debug") {
            DEBUG = true;
        }
        else if (std::string(argv[i]) == "-dump") {
            DUMP = true;
        }

        else if (std::string(argv[i]).substr(0, 2) == "0x") {
            std::stringstream ss;
            ss << std::hex << std::string(argv[i]).substr(2, 2);
            ss >> testOpcode;
        }
        else if(is_number(std::string(argv[i]))) {
            std::string s = argv[i];
            testAmount = std::stoi(s);
        }
        else if(i > 2) {
            std::cout << "parameter: " << argv[i] << " not recognized" << std::endl;
            return -1;
        }
    }

    if (isTest) {
        MEM = (uint8_t*)malloc(sizeof(uint8_t) * 0x10000);
        memset(MEM, 0, sizeof(uint8_t) * 0x10000);
        std::cout << "Executing test on Tom Harte's 6502 JSON files (https://github.com/TomHarte/ProcessorTests/tree/main/6502)" << std::endl << std::endl;
        testInstructions();
    }
    else {
        if (ROMpath == nullptr) {
            ROMpath = argv[1];
        }

        // We do not talk about this
        appPath = std::string(argv[0]);
        appPath = appPath.substr(0, appPath.size() - 7);
        MEM = (uint8_t*)malloc(sizeof(uint8_t) * 0x2000);
        memset(MEM, 0, sizeof(uint8_t) * 0x2000);

        if (ROMpath != nullptr) {
            loadCartridge(ROMpath);
        }
        else {
            std::cout << "No rom selected" << std::endl;
            return -1;
        }

        initAudio();
        init6507();
        initTIA();
        initPIA();

        EmuGraphics::initScreen();
        std::thread Atarithread(Atariloop);

        if (DEBUG) {
            std::thread GraphicsThread(EmuGraphics::loopScreen);
            initDebug();
            debugLog();
        }
        else {
            EmuGraphics::loopScreen();
            Atarithread.~thread();
            EmuGraphics::closeScreen();
            endAudio();
            free(MEM);
        }
    }
}