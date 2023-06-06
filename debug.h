#ifndef ATARIDEBUG_H
#define ATARIDEBUG_H

#include<stack>
#include<iostream>
#include "6507.h"
#include "imgui.h"
#include "imgui-SFML.h"
#include<vector>
#include <iostream>
#include<sstream>
#include <bitset>
#include <iomanip>
#include <windows.h>
#include <map>
#include "PIA.h"

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/OpenGL.hpp>

struct opCall {
public:
    uint16_t opAddr;
    uint8_t opArg;
    uint8_t opCode;

    opCall(uint16_t OpAddr, uint8_t OpArg, uint8_t OpCode)
    {
        opAddr = OpAddr;
        opArg = OpArg;
        opCode = OpCode;
    }
};

extern sf::RenderWindow* window;
extern ImFont* font1;
extern std::map<std::uint8_t, std::string> mnemonics;

extern std::vector<opCall> callStack;


extern bool cycleButton;
extern bool stepLineButton;
extern bool stepFrameButton;


void debugLog();

void initDebug();

void logTIA();

void logPIA();

void logRegisters();

void logCallStack();

void logStack();

void initButtons();

template< typename T >
std::string getHex(T nothex);
std::string getBin(int notbin);
uint16_t Pin16To13(uint16_t pin16);

void endDebug();

#endif