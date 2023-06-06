#include "6507addressingModes.h"

AM AddrMode;

void AM_IMP() {
    AddrMode = AM::IMP;
}

void AM_IMM() {
    AddrMode = AM::IMM;
}

void AM_ABS() {
    AddrMode = AM::ABS;
}

void AM_ZRO() {
    AddrMode = AM::ZRO;
}

void AM_INX() {
    AddrMode = AM::INX;
}

void AM_INY() {
    AddrMode = AM::INY;
}

void AM_ZRX() {
    AddrMode = AM::ZRX;
}

void AM_ZRY() {
    AddrMode = AM::ZRY;
}

void AM_IND() {
    AddrMode = AM::IND;
}

void AM_INDX() {
    AddrMode = AM::INDX;
}

void AM_INDY() {
    AddrMode = AM::INDY;
}

void AM_REL() {
    AddrMode = AM::REL;
}