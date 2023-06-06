#ifndef TIA_H
#define TIA_H

#include<iostream>
#include "6507.h"
#include "EmuGraphics.h"
#include <ctime>
#include <ratio>
#include <chrono>
#include <fstream>
#include <iterator>
#include <bitset>
#include "EmuAudio.h"

extern void (*TIASTROBEMEM[0x3E])();		// 0000-003D (waste of space since there's only 10 TIA strobes, should find better mapping method)
extern uint8_t PMBPOS[5];					// P0, P1, M0, M1, BL latched internally, wraps around at 160
extern uint8_t GRPdelayed[2];				// Used for vertical delay, transefered to actual GRPx when writing to GRPy
extern uint8_t ENABLdelayed;				// Used for vertical delay, transefered to actual ENABL when writing to GRP1
extern bool objectsDrawn[6];				// What objects were drawn on the current pixel, used for collisions (P0, P1, M0, M1, BL, PF)
extern int8_t HMtable[1248];				// Holds the offsets applied when calling HMOVE outside of HBLANK


extern uint8_t CRT_VBLANK;
extern uint8_t CRT_HBLANK;
extern uint8_t CRT_OVERSCAN;
extern uint16_t CRT_VSCREEN;
extern uint8_t CRT_HSCREEN;
extern uint8_t CRT_VSYNC;

extern uint16_t clockCount;	// Combination of scanline and colorclock (scanline*CRTWIDTH + colorclock), 3 colorclocks per 6507 cycle

extern uint8_t NTSCcolors[0xFF * 3];	// *3 for RGB channels
// RESxx uses the colorclock from when the RESxx strobe has finished calling
// But since my emulator only applies the duration of the instruction to the color clock
// After it has completed the CPU tick (and thus read from the colorclock and applied it to pos)
// It needs to be scheduled to be called AFTER it has completed the TIA ticks
// And thus applied the duration to the color clock, so this needs to be called
// right before the next CPU tick, as that is when the TIA ticks that belong to the 
// Previous CPU tick have been performed
extern void (*bufferedRESxx)();
// Same for REsxx, except for writes to player graphics
// This is because the writes to these addresses needs to be very precise
// For correctly displaying text and scores
// I am aware that this is a significant issue in the emulator
// but these are really the only places where such precision is needed afaik
extern int16_t bufferedGRP[2];





void initTIA();
void doBufferedTIA();
void initNTSC();
void initHMOVE();
void TIAWrite(uint16_t loc, uint8_t val);
void tickTIA();
void drawPF_TIA();
void drawBG_TIA();
void drawMx_TIA(uint8_t Mx);
void drawBL_TIA();
void drawPx_TIA(uint8_t Px);

void setCollisions_TIA();

uint8_t TIARead(uint16_t loc);

void RESMPx_TIA(uint8_t Px);

// Strobe functions 
void TIA_WSYNC();	// Wait for leading edge of horizontal blank
void TIA_RSYNC();	// Reset horizontal sync counter

void TIA_RESP0();	// Reset player 0
void TIA_RESP1();	// Reset player 1
void TIA_RESM0();	// Reset missile 0
void TIA_RESM1();	// Reset missile 1
void TIA_RESBL();	// Reset ball

// See bufferedResxx above for explanation.
void TIA_RESP0_BUFFR();
void TIA_RESP1_BUFFR();
void TIA_RESM0_BUFFR();
void TIA_RESM1_BUFFR();
void TIA_RESBL_BUFFR();

void TIA_HMOVE();	// Apply horizontal motion
void TIA_HMCLR();	// Clear horizontal motion registers
void TIA_CXCLR();	// Clear collision latches


void setScreenPixel(uint16_t loc, uint8_t* col);
int8_t HMtoOffset(uint8_t HMbyte);	// Handles weird HMOVE outside of HBLANK
uint8_t* getNTSCcolor(uint8_t col);	// Returns NTSC RGB channels
uint16_t getColorClock();	// Gets clockCount relative to start of scanline
uint16_t getVclock();		// Gets clockCount at if colorClock is at start of scanline
uint16_t getScanLine();
uint8_t getGraphicPos(uint8_t PMB);	// Selects from PMBPOS, wrapping around 160
void setGraphicPos(uint8_t PMB, uint16_t newPos);
void changeGraphicPos(uint8_t PMB, int16_t deltaPos);






#endif
