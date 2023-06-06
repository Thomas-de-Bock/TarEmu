#include "6507opcodes.h"

void (*LUT[0xFF])();
uint16_t arg;


//==========
// Loading =
//==========

void OP_LDX() {
    X = GETMEM8(arg);

    //Flag Zero (1) and Sign (7) are set here
    //      Sign   Zero
    //        |      |
    P = P & 0b01111101; // Zero out flag 1 and 7, keep the rest unchanged
    //  0ppp pp0p
    P = P | (X == 0 ? 0b00000010 : 0); // Set zero bit if X is 0
    //  0ppp pp0p
    //  0000 0010
    //  --------- = 0ppp pp1p
    P = P | (X & 0b10000000); // Set Sign (7) bit if X bit 7 is 1
    //  xxxx xxxx
    //  1000 0000
    //  --------- = x000 0000
    //              0ppp pp1p
    //              --------- = xppp pp1p
}

void OP_LDY() {
    Y = GETMEM8(arg);
    
    P = P & 0b01111101;                 // Zero out
    P = P | (Y == 0 ? 0b00000010 : 0);  // Zero flag
    P = P | (Y & 0b10000000);           // Sign flag
}

void OP_LDA() {
    A = GETMEM8(arg);

    P = P & 0b01111101;                 // Zero out
    P = P | (A == 0 ? 0b00000010 : 0);  // Zero flag
    P = P | (A & 0b10000000);           // Sign flag
}


//==========
// Storing =
//==========

void OP_STX() {
    SETMEM8(arg, X);

    //No status change
}
void OP_STY() {
    SETMEM8(arg, Y);

    //No status change
}
void OP_STA() {
    SETMEM8(arg, A);

    //No status change
}


//==========
//  Trans  =
//==========

void OP_TAX() {
    X = A;
    
    P = P & 0b01111101;                 // Zero out
    P = P | (X == 0 ? 0b00000010 : 0);  // Zero flag
    P = P | (X  & 0b10000000);          // Sign flag
}
void OP_TAY() {
    Y = A;

    P = P & 0b01111101;                 // Zero out
    P = P | (Y == 0 ? 0b00000010 : 0);  // Zero flag
    P = P | (Y  & 0b10000000);          // Sign flag
}
void OP_TSX() {
    X = S;

    P = P & 0b01111101;                 // Zero out
    P = P | (X == 0 ? 0b00000010 : 0);  // Zero flag
    P = P | (X  & 0b10000000);          // Sign flag
}
void OP_TXA() {
    A = X;

    P = P & 0b01111101;                 // Zero out
    P = P | (A == 0 ? 0b00000010 : 0);  // Zero flag
    P = P | (A  & 0b10000000);          // Sign flag
}
void OP_TXS() {
    S = X;

    // No status changed
}
void OP_TYA() {
    A = Y;

    P = P & 0b01111101;                 // Zero out
    P = P | (A == 0 ? 0b00000010 : 0);  // Zero flag
    P = P | (A  & 0b10000000);          // Sign flag
}


//==========
//  Stack  =
//==========

void OP_PHA() {
    SETRAM8(S--, A);

    // No status changed
}
void OP_PHP() {
    SETRAM8(S--, P|0b00110000);

    // No status changed
}
void OP_PLA() {
    A = GETRAM8(++S);

    P = P & 0b01111101;                 // Zero out
    P = P | (A == 0 ? 0b00000010 : 0);  // Zero flag
    P = P | (A  & 0b10000000);          // Sign flag
}
void OP_PLP() {
    P = GETRAM8(++S);
    P = P|0b00100000;    // Byte 5 ignored (set)
    P = P&0b11101111;    // Break flag ignored (cleared)

    // Function changes status register directly
}

//==========
//  Shift  =
//==========

void OP_ASL() {
    P = P & 0b01111100;                     // Zero out
    if (AddrMode == AM::IMP) {
        P = P | ((A & 0b10000000)>>7);           // Carry flag
        A = A << 1;
        P = P | (A  & 0b10000000);          // Sign flag
        P = P | (A == 0 ? 0b00000010 : 0);  // Zero flag
    } else {
        P = P | ((GETMEM8(arg) & 0b10000000)>>7);           // Carry flag
        SETMEM8(arg, GETMEM8(arg) << 1);
        P = P | (GETMEM8(arg)  & 0b10000000);          // Sign flag
        P = P | (GETMEM8(arg) == 0 ? 0b00000010 : 0);  // Zero flag
    }
}
void OP_LSR() {
    P = P & 0b01111100;                     // Zero out
    // Sign bit is set to 0, couldn't be 1 anyway because of shift
    if (AddrMode == AM::IMP) {
        P = P | (A & 0b00000001);           // Carry flag
        A = A >> 1;
        P = P | (A == 0 ? 0b00000010 : 0);  // Zero flag
    } else {
        P = P | (GETMEM8(arg) & 0b00000001);           // Carry flag
        P = P | (GETMEM8(arg) & 0b00000001);           // Carry flag
        SETMEM8(arg, GETMEM8(arg) >> 1);
        P = P | (GETMEM8(arg) == 0 ? 0b00000010 : 0);  // Zero flag
    }
}
void OP_ROL() {
    if (AddrMode == AM::IMP) {
        uint8_t noShiftA = A;
        A = (A << 1) | (P &0b00000001);
        P = P & 0b01111100;                     // Zero out
        P = P | (A == 0 ? 0b00000010 : 0);      // Zero flag
        P = P | (A & 0b10000000);               // Sign flag
        P = P | ((noShiftA & 0b10000000) >> 7); // Carry flag
    } else {
        uint8_t noShiftMEM = GETMEM8(arg);
        SETMEM8(arg, (noShiftMEM << 1) | (P & 0b00000001));
        P = P & 0b01111100;                             // Zero out
        P = P | (GETMEM8(arg) == 0 ? 0b00000010 : 0);   // Zero flag
        P = P | (GETMEM8(arg) & 0b10000000);            // Sign flag
        P = P | ((noShiftMEM & 0b10000000) >> 7);       // Carry flag
    }
}
void OP_ROR() {
    if (AddrMode == AM::IMP) {
        uint8_t noShiftA = A;
        uint8_t noShiftP = P;                   // Status different from ROL
        A = (A >> 1) | ((P & 0b00000001)<<7);
        P = P & 0b01111100;                     // Zero out
        P = P | (A == 0 ? 0b00000010 : 0);      // Zero flag
        P = P | ((noShiftP & 0b00000001)<<7);   // Sign flag
        P = P | (noShiftA & 0b00000001);        // Carry flag
    } else {
        uint8_t noShiftMEM = GETMEM8(arg);
        uint8_t noShiftP = P;                   // Status different from ROL
        SETMEM8(arg, (noShiftMEM >> 1) | ((P & 0b00000001) << 7));
        P = P & 0b01111100;                     // Zero out
        P = P | (GETMEM8(arg) == 0 ? 0b00000010 : 0);      // Zero flag
        P = P | ((noShiftP & 0b00000001) << 7);   // Sign flag
        P = P | (noShiftMEM & 0b00000001);        // Carry flag
    }
}

//==========
//  Logic  =
//==========

void OP_AND() {
    A = A & GETMEM8(arg);

    P = P & 0b01111101;                 // Zero out
    P = P | (A == 0 ? 0b00000010 : 0);  // Zero flag
    P = P | (A & 0b10000000);           // Sign flag
}
void OP_BIT() {
    uint8_t testBits = A & GETMEM8(arg);

    P = P & 0b00111101;                         // Zero out
    P = P | (GETMEM8(arg) & 0b10000000);        // Sign flag
    P = P | (GETMEM8(arg) & 0b01000000);        // OVerflow flag
    P = P | (testBits == 0 ? 0b00000010 : 0);   // Zero flag
}
void OP_EOR() {
    A = GETMEM8(arg) ^ A; // ^ Is exclusive OR

    P = P & 0b01111101;                         // Zero out
    P = P | (A == 0 ? 0b00000010 : 0);          // Zero flag
    P = P | (A & 0b10000000);                   // Sign flag
}
void OP_ORA() {
    A = GETMEM8(arg) | A;

    P = P & 0b01111101;                         // Zero out
    P = P | (A == 0 ? 0b00000010 : 0);          // Zero flag
    P = P | (A & 0b10000000);                   // Sign flag
}

//==========
//  Arith  =
//==========

void OP_ADC() { // Someone please fix decimal mode
    int16_t res = 0;

    if((P & 0b00001000) == 0) {
        res = A + GETMEM8(arg) + (0b00000001 & P);
        P = P & 0b00111100;                                 // Zero out
        P = P | (res > 0xFF ? 0b00000001 : 0);                         // Carry flag
        P = P | ((~(A ^ GETMEM8(arg)) & (A ^ res) & 0x80) ? 0b01000000 : 0);   // Overflow flag
        A = res;
        P = P | (A & 0b10000000);                                       // Sign flag
        P = P | (A == 0 ? 0b00000010 : 0);                              // Zero flag
    } else {
        uint8_t resL = (A & 0x0F) + (GETMEM8(arg) & 0x0F) + (0b00000001 & P);
        res = (A & 0xF0) + (GETMEM8(arg) & 0xF0) + resL;
        P = P | (res == 0 ? 0b00000010 : 0);                // Zero flag  "The Z flag is computed before performing any decimal adjust."
        resL += resL > 9 ? 0x06 : 0;
        uint16_t resH = (A & 0xF0) + (GETMEM8(arg) & 0xF0);
        res = resH + resL;
        P = P | ((res < -128 || res>127) ? 0b01000000 : 0);   // Overflow flag
        P = P | (A & 0b10000000);                             // Sign flag
        res += res > 0x9F ? 0x60 : 0;
        A = res;

        P = P | (res > 0xFF ? 0b00000001 : 0);                // Carry flag
    }


}
void OP_CMP() {
    P = P & 0b01111100;                                      // Zero out
    P = P | (A-GETMEM8(arg) == 0 ? 0b00000010 : 0);          // Zero flag
    P = P | (A-GETMEM8(arg) & 0b10000000);                   // Sign flag
    P = P | (A-GETMEM8(arg) >= 0 ? 0b00000001 : 0);          // Carry flag
}
void OP_CPX() {
    P = P & 0b01111100;                                         // Zero out
    P = P | (X-GETMEM8(arg) == 0 ? 0b00000010 : 0);             // Zero flag
    P = P | (X-GETMEM8(arg) & 0b10000000);                      // Sign flag
    P = P | (X-GETMEM8(arg) >= 0 ? 0b00000001 : 0);             // Carry flag
}
void OP_CPY() {
    P = P & 0b01111100;                                         // Zero out
    P = P | (Y-GETMEM8(arg) == 0 ? 0b00000010 : 0);             // Zero flag
    P = P | (Y-GETMEM8(arg) & 0b10000000);                      // Sign flag
    P = P | (Y-GETMEM8(arg) >= 0 ? 0b00000001 : 0);             // Carry flag
}
void OP_SBC() {
    int16_t res;
    if ((P & 0b00001000) == 0) {
        res = A + (~GETMEM8(arg)) + (0b00000001 & P);
        P = P & 0b00111100;                                 // Zero out
        P = P | (res >= 0 ? 0b00000001 : 0);                            // Carry flag
        P = P | ((~(A ^ (~GETMEM8(arg))) & (A ^ res) & 0x80) ? 0b01000000 : 0);   // Overflow flag
        A = res;
        P = P | (A & 0b10000000);                                       // Sign flag
        P = P | (A == 0 ? 0b00000010 : 0);                              // Zero flag
    }
    else {
        res = (A & 0x0F) + (~GETMEM8(arg) & 0x0F) + (0b00000001 & P);
        if (res <= 0x0F) {
            res -= 0x06;
        }
        res = (A & 0xF0) + (~GETMEM8(arg) & 0xF0) + (res > 0x0F ? 0x10 : 0) + (res & 0x0F);
        P = P | ((res < -128 || res>127) ? 0b01000000 : 0);   // Overflow flag
        P = P | (res > 0xFF ? 0b00000001 : 0);                // Carry flag
        if (res > A) {  // Means it overflowed
            res = (uint8_t)res;
            if (res >= 0xA0) {
                res -= 0x60;
            }
        }
        A = (uint8_t)res;
        P = P | (A & 0b10000000);                                       // Sign flag
        P = P | (A == 0 ? 0b00000010 : 0);                              // Zero flag
    }
    

}

//==========
// inc/dec =
//==========

void OP_DEC() {
    SETMEM8(arg, GETMEM8(arg) - 1);

    P = P & 0b01111101;                                     // Zero out
    P = P | (GETMEM8(arg) & 0b10000000);                    // Sign flag
    P = P | (GETMEM8(arg) == 0 ? 0b00000010 : 0);           // Zero flag
}
void OP_DEX() {
    X--;

    P = P & 0b01111101;                                 // Zero out
    P = P | (X & 0b10000000);                           // Sign flag
    P = (X == 0) ? (P | 0b00000010) : P;                // Zero flag
}
void OP_DEY() {
    Y--;

    P = P & 0b01111101;                                 // Zero out
    P = P | (Y & 0b10000000);                           // Sign flag
    P = (Y == 0) ? (P | 0b00000010) : P;                // Zero flag
}
void OP_INC() {
    SETMEM8(arg, GETMEM8(arg)+1);

    P = P & 0b01111101;                                 // Zero out
    P = P | (GETMEM8(arg) & 0b10000000);                    // Sign flag
    P = (GETMEM8(arg) == 0) ? (P | 0b00000010) : P;         // Zero flag
}
void OP_INX() {
    X++;

    P = P & 0b01111101;                                 // Zero out
    P = P | (X & 0b10000000);                           // Sign flag
    P = (X == 0) ? (P | 0b00000010) : P;                // Zero flag
}
void OP_INY() {
    Y++;

    P = P & 0b01111101;                                 // Zero out
    P = P | (Y & 0b10000000);                           // Sign flag
    P = (Y == 0) ? (P | 0b00000010) : P;                // Zero flag
}

//==========
// Control =
//==========

void OP_BRK() {
    SETRAM16(S--, PC+1);
    SETRAM8(--S, P | 0b00010000);       // Store status reg on stack with break flag
    S--;
    P = P | 0b00000100;                 // Interrupt vector
    PC = (((uint16_t)(GETMEM8(0xFFFF))) << 8) | GETMEM8(0xFFFE);    // Jump to interrupt vector
}
void OP_JMP() {
    // One of only cases where adressing mode needs to be handled in function
    // This is due to the unfortunate existence of Indirect Addressing
    // Which is the only adressing mode that can return a final value of
    // 2 bytes, and it is ONLY used in the JMP operation
    if (AddrMode == AM::IND) {
        // Bug on 6502 when indirect ptr to page boundary, fixed on some later chips
        // I'm assuming the 6507 doesn't, based on some sources stating they're compatible
        // While specifying others (like the 65C02, which does have the fix) are not.
        // In the bug, on ind ptr to page boundary, 
        // The address pointing at the second byte is wrapped, which is why in my implementation
        // If the argL is on a page boundary, what would be arg+1, is just argH as argL is wrapped
        // around to 00.
        uint16_t argH = arg & 0xFF00;
        uint8_t argL = arg & 0x00FF;

        if (argL == 0xFF) {
            uint16_t indArg = ((uint16_t)GETMEM8(argH) << 8) | ((uint16_t)GETMEM8(arg));
            PC = indArg;
        }
        else {
            PC = GETMEM16(arg);
        }
        return;
    }
    PC = arg;
}
void OP_JSR() {
    SETRAM16(S--, PC-1);

    S--;                                // PC is 16 bit so add 1 more offset
    PC = arg;
}
void OP_RTI() { //interupt is IRQ or smn?
    P = GETRAM8(++S) & 0b11001111;      // Restore status reg
    P = P | 0b00100000;                 // Byte 5 is set
    PC = GETRAM16(++S);                 // Restore PC
    S++;                                // PC is 16 bit so add 1 more offset
}
void OP_RTS() {
    PC = GETRAM16(++S)+1;                 // Restore PC    
    S++;                                  // PC is 16 bit so add 1 more offset
}

//==========
//  Branch =
//==========
// Converted to signed int8's for all PC offsets on conditional branching
void OP_BCC() {
    //Inverse of BCS
    uint16_t oldPC = PC;
    PC += (P & 0b00000001) == 0x01 ? 0 : (int8_t)(GETMEM8(arg));
    if (oldPC == PC) {
        return;
    }
    if (getPage(PC) == getPage(oldPC)) {
        cycle6507(1);
    }
    else {
        cycle6507(2);
    }
}
void OP_BCS() { 
    //Inverse of BCC
    uint16_t oldPC = PC;
    PC += (P & 0b00000001) == 0x01 ? (int8_t)(GETMEM8(arg)) : 0;
    if (oldPC == PC) {
        return;
    }
    if (getPage(PC) == getPage(oldPC)) {
        cycle6507(1);
    }
    else {
        cycle6507(2);
    }
}
void OP_BEQ() {
    //Inverse of BNE
    uint16_t oldPC = PC;
    PC += (P & 0b00000010) == 0x02 ? (int8_t)(GETMEM8(arg)) : 0;
    if (oldPC == PC) {
        return;
    }
    if (getPage(PC) == getPage(oldPC)) {
        cycle6507(1);
    }
    else {
        cycle6507(2);
    }
}
void OP_BMI() {
    //Inverse of BPL
    uint16_t oldPC = PC;
    PC += (P & 0b10000000) == 0x80 ? (int8_t)(GETMEM8(arg)) : 0;
    if (oldPC == PC) {
        return;
    }
    if (getPage(PC) == getPage(oldPC)) {
        cycle6507(1);
    }
    else {
        cycle6507(2);
    }
}
void OP_BNE() {
    // Inverse of BEQ
    uint16_t oldPC = PC;
    PC += (P & 0b00000010) == 0x02 ? 0 : (int8_t)(GETMEM8(arg));
    if (oldPC == PC) {
        return;
    }
    if (getPage(PC) == getPage(oldPC-1)) {
        cycle6507(1);
    }
    else {
        cycle6507(2);
    }
}
void OP_BPL() {
    //Inverse of BMI
    uint16_t oldPC = PC;
    PC += (P & 0b10000000) == 0x80 ? 0 : (int8_t)(GETMEM8(arg));
    if (oldPC == PC) {
        return;
    }
    if (getPage(PC) == getPage(oldPC-1)) {
        cycle6507(1);
    }
    else {
        cycle6507(2);
    }
}
void OP_BVC() {
    //Inverse of BVS
    uint16_t oldPC = PC;
    PC += (P & 0b01000000) == 0x40 ? 0 : (int8_t)(GETMEM8(arg));
    if (oldPC == PC) {
        return;
    }
    if (getPage(PC) == getPage(oldPC-1)) {
        cycle6507(1);
    }
    else {
        cycle6507(2);
    }
}
void OP_BVS() {
    //Inverse of BVC
    uint16_t oldPC = PC;
    PC += (P & 0b01000000) == 0x40 ? (int8_t)(GETMEM8(arg)) : 0;
    if (oldPC == PC) {
        return;
    }
    if (getPage(PC) == getPage(oldPC-1)) {
        cycle6507(1);
    }
    else {
        cycle6507(2);
    }
}

//==========
//  Flags  =
//==========
void OP_CLC() {
    P = P & 0b11111110;
}
void OP_CLD() {
    P = P & 0b11110111;
}
void OP_CLI() {
    P = P & 0b11111011;
}
void OP_CLV() {
    P = P & 0b10111111;
}
void OP_SEC() {
    P = P | 0b00000001;
}
void OP_SED() {
    P = P | 0b00001000;
}
void OP_SEI() {
    P = P | 0b00000100;
}

//==========
//   NOP   =
//==========
void OP_NOP() {
    // No operation
}