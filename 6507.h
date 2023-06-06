#ifndef CPU_H
#define CPU_H

#include "6507addressingModes.h"
#include "6507opcodes.h"
#include "debug.h"
#include <thread>
#include "debug.h"
#include "TIA.h"
#include <ctime>
#include <ratio>
#include <chrono>
#include "PIA.h"
#include "cartridge.h"

//==========================================
// Registers        Bits: | Expl:
//==========================================
extern uint8_t A;  //8     Accumulator
extern uint8_t X;  //8     Index Register X  
extern uint8_t Y;  //8     Index Register Y
extern uint8_t S;  //8     Stack Pointer
extern uint16_t PC;//16    Program Counter
extern uint8_t P;  //8     Processor Status Register
                            /*
                            Flags
                            Bit  Name  Expl.
                            0    C     Carry         (0=No Carry, 1=Carry)
                            1    Z     Zero          (0=Nonzero, 1=Zero)
                            2    I     IRQ Disable   (0=IRQ Enable, 1=IRQ Disable)
                            3    D     Decimal Mode  (0=Normal, 1=BCD Mode for ADC/SBC opcodes)
                            4    B     Break Flag    (0=IRQ/NMI, 1=RESET or BRK/PHP opcode)
                            5    -     Not used      (Always 1)
                            6    V     Overflow      (0=No Overflow, 1=Overflow)
                            7    N     Negative/Sign (0=Positive, 1=Negative)
                            N, V, -, B, D, I, Z, C
                            */
extern uint8_t* MEM; // 0x2000 allocated for emulation, 0x10000 for testing
extern bool DEBUG;
extern bool CPUTEST;
extern uint16_t RAMoffset;  // Different on 6502 in NES
extern int cycles;
extern int steps;           // Only implemented for dumping states
extern bool cycleException;
extern bool RDY;
extern uint32_t cycleTime6507;   // In nanoseconds
extern bool DUMP;
extern uint8_t currentOpcode;    // Used for mapping reads from write-only to readable addresses

//==========================================
// Basic CPU operational functions
//==========================================
extern void init6507();
void initLUT6507();
int tick6507();

void cycle6507(int cycles);

template<void (*adressingModePtr)(), void (*functionPtr)(), int cycleAmount, bool OPCycleException>
void opcode();
void getArg();  // Expects Program counter to be on first byte after operator

uint16_t GETMEM16(uint16_t loc);
uint8_t GETMEM8(uint16_t loc);
uint16_t GETRAM16(uint8_t loc);
uint8_t GETRAM8(uint8_t loc);

void SETMEM16(uint16_t loc, uint16_t val);
void SETMEM8(uint16_t loc, uint8_t val);
void SETRAM16(uint8_t loc, uint16_t val);
void SETRAM8(uint8_t loc, uint8_t val);


int16_t getPage(uint16_t address);
uint8_t reverseByte(uint8_t inpByte);


#endif
