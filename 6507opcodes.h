#ifndef OPCODES_H
#define OPCODES_H

#include<iostream>
#include "6507.h"
#include "6507addressingModes.h"
#include <bitset>
#include <cmath>

extern void (*LUT[0xFF])();
extern uint16_t arg;

//Loading
void OP_LDX();
void OP_LDY();
void OP_LDA();

//Storing
void OP_STX();
void OP_STY();
void OP_STA();

//Trans
void OP_TAX();
void OP_TAY();
void OP_TSX();
void OP_TXA();
void OP_TXS();
void OP_TYA();


//Stack
void OP_PHA();
void OP_PHP();
void OP_PLA();
void OP_PLP();

//Shift
void OP_ASL();
void OP_LSR();
void OP_ROL();
void OP_ROR();

//Logic
void OP_AND();
void OP_BIT();
void OP_EOR();
void OP_ORA();

//Arithmatic
void OP_ADC();
void OP_CMP();
void OP_CPX();
void OP_CPY();
void OP_SBC();

//Increment/Decrement
void OP_DEC();
void OP_DEX();
void OP_DEY();
void OP_INC();
void OP_INX();
void OP_INY();

//Control
void OP_BRK();
void OP_JMP();
void OP_JSR();
void OP_RTI();
void OP_RTS();

//Branch
void OP_BCC();
void OP_BCS();
void OP_BEQ();
void OP_BMI();
void OP_BNE();
void OP_BPL();
void OP_BVC();
void OP_BVS();

//Flags
void OP_CLC();
void OP_CLD();
void OP_CLI();
void OP_CLV();
void OP_SEC();
void OP_SED();
void OP_SEI();

//NOP
void OP_NOP();





#endif

