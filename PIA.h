#ifndef PIA_H
#define PIA_H

#include<iostream>
#include "6507.h"
#include<SFML/System.hpp>

extern uint16_t INTIMclocks;
extern uint16_t currentInterval;
extern uint16_t normalInterval;
extern int totalTicks;	// This has no reason to be here
extern bool f3Pressed;	// If f3 was held on the last frame

void updateInput();
void initPIA();
void PIAwrite(uint16_t loc, uint8_t val);
uint8_t PIAread(uint16_t loc);
void tickPIA();



#endif