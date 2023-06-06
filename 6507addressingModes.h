#ifndef ADRESSINGMODES_H
#define ADRESSINGMODES_H

enum AM {
    IMP,
    IMM,
    ABS,
    ZRO,
    INX,
    INY,
    ZRX,
    ZRY,
    IND,
    INDX,
    INDY,
    REL
};

extern AM AddrMode;

void AM_IMP(); 
//Implied Addressing:
//
void AM_IMM();
//Immediate Addressing:
//
void AM_ABS();
//Absolute Addressing:
//
void AM_ZRO();
//Zero-Page Addressing:
//
void AM_INX();
//Indexed Addressing (Absolute X):
//
void AM_INY();
//Indexed Addressing(Absolute Y):
//
void AM_ZRX();
//Indexed Addressing (Zero-Page X):
//
void AM_ZRY();
//Indexed Addressing (Zero-Page Y);
//
void AM_IND();
//Indirect Addressing
//
void AM_INDX();
//Pre-Indexed Indirect Addressing (Zero-Page X)
//
void AM_INDY();
//Post-Indexed Indirect Addressing (Zero-Page Y)
//
void AM_REL();
//Relative Addressing (Conditional Branching)
//

#endif



