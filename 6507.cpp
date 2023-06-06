#include "6507.h"

uint8_t A;
uint8_t X;
uint8_t Y;
uint8_t S;
uint8_t P;
uint16_t PC;

uint8_t* MEM;

bool DEBUG = false;
bool CPUTEST = false;
bool cycleException = false;
bool RDY = true;
bool DUMP = false;

int cycles;
int steps;

uint16_t RAMoffset;

uint32_t cycleTime6507 = 840;
uint8_t currentOpcode;

void init6507() {
    //==========================================
    // Setup CPU values
    //==========================================
    PC = (((uint16_t)(GETMEM8(0xFFFD))) << 8) | GETMEM8(0xFFFC);    // ROM lives at 0xF000-0xFFFF, start vector at: 0xFFFC-0xFFFD
    
    P = 0b00100000; // Byte 5 set
    A = 0x00;
    Y = 0x00;
    S = 0xFD;       // RAM lives at 0x0080 - 0x00FF


    cycles = 0;
    arg = 0x00;

    // Only used in jsonTest (NES applies offset on stack ptr)
    RAMoffset = CPUTEST ? 0x100 : 0;

    if (DUMP) {
        // Matches my latest dump of whatever game I tried, insert own data for debugging purposes
        S = 0xFD;
        A = 0x75;
        X = 0x8D;
        Y = 0x6E;
        P = 0b11010010;

        uint8_t dumpMap[0x80] = { 0x75, 0xa4, 0xc7, 0x38, 0x78, 0x04, 0x64, 0x63, 0xe4, 0x2b, 0xdb, 0x05, 0x37, 0xdd, 0x61, 0x8c
, 0xf8, 0xf6, 0x18, 0x51, 0x90, 0x32, 0x74, 0xab, 0xf0, 0xbb, 0x2c, 0x09, 0xcd, 0x54, 0x02, 0xba
, 0x68, 0xad, 0xb4, 0x18, 0x6e, 0xc8, 0x27, 0x3f, 0x97, 0xc1, 0xbb, 0x0f, 0xd1, 0xae, 0xea, 0xba
, 0xf7, 0xdb, 0x39, 0x18, 0x99, 0x58, 0xb5, 0x0c, 0x27, 0xb0, 0x06, 0x75, 0xb9, 0x88, 0x4f, 0xf6
, 0x8c, 0x56, 0x30, 0xad, 0x3e, 0x52, 0x7d, 0x98, 0x85, 0xb2, 0xdd, 0x5d, 0x94, 0xc4, 0xaa, 0x64
, 0x6f, 0xb6, 0x04, 0x76, 0x79, 0x87, 0x77, 0x3a, 0x13, 0x32, 0xfd, 0x0e, 0x19, 0xb0, 0x63, 0x21
, 0xaa, 0x3b, 0xc6, 0x64, 0xf0, 0x27, 0x32, 0xc6, 0x4b, 0x36, 0xf6, 0xb1, 0x6b, 0x89, 0xcf, 0x0c
, 0x52, 0x68, 0xd9, 0x8f, 0xe2, 0x0a, 0x91, 0x8b, 0x6b, 0x4c, 0x8c, 0xdb, 0x39, 0x79, 0x3f, 0xac };

        for (int i = 0; i < 0x80; i++) {
            MEM[i + 0x80] = dumpMap[i];
        }
    }

    initLUT6507();
}

void initLUT6507() {
    //==========================================
    // Map all opcodes to the lookup table (LUT)
    // Illegal opcodes not included
    //==========================================
    //
    //  Opcode          AddMode mnemonic cycles  (add cycle if page boundary crossed)
    //    |                |        |    |                      /
    //    |                |        |    |     ----------------
    //    |                |        |    |    |
    LUT[0x69] = opcode<&AM_IMM, &OP_ADC, 2, false>;
    LUT[0x65] = opcode<&AM_ZRO, &OP_ADC, 3, false>;
    LUT[0x75] = opcode<&AM_ZRX, &OP_ADC, 4, false>;
    LUT[0x6D] = opcode<&AM_ABS, &OP_ADC, 4, false>;
    LUT[0x7D] = opcode<&AM_INX, &OP_ADC, 4, true>;          // Add 1 to cycles if page boundary is crossed
    LUT[0x79] = opcode<&AM_INY, &OP_ADC, 4, true>;          // Add 1 to cycles if page boundary is crossed
    LUT[0x61] = opcode<&AM_INDX, &OP_ADC, 6, false>;
    LUT[0x71] = opcode<&AM_INDY, &OP_ADC, 5, true>;         // Add 1 to cycles if page boundary is crossed
    LUT[0x29] = opcode<&AM_IMM, &OP_AND, 2, false>;
    LUT[0x25] = opcode<&AM_ZRO, &OP_AND, 3, false>;
    LUT[0x35] = opcode<&AM_ZRX, &OP_AND, 4, false>;
    LUT[0x2D] = opcode<&AM_ABS, &OP_AND, 4, false>;
    LUT[0x3D] = opcode<&AM_INX, &OP_AND, 4, true>;          // Add 1 to cycles if page boundary is crossed
    LUT[0x39] = opcode<&AM_INY, &OP_AND, 4, true>;          // Add 1 to cycles if page boundary is crossed
    LUT[0x21] = opcode<&AM_INDX, &OP_AND, 6, false>;
    LUT[0x31] = opcode<&AM_INDY, &OP_AND, 5, true>;         // Add 1 to cycles if page boundary is crossed
    LUT[0x0A] = opcode<&AM_IMP, &OP_ASL, 2, false>;
    LUT[0x06] = opcode<&AM_ZRO, &OP_ASL, 5, false>;
    LUT[0x16] = opcode<&AM_ZRX, &OP_ASL, 6, false>;
    LUT[0x0E] = opcode<&AM_ABS, &OP_ASL, 6, false>;
    LUT[0x1E] = opcode<&AM_INX, &OP_ASL, 7, false>;
    LUT[0x90] = opcode<&AM_REL, &OP_BCC, 2, false>;         // Add 1 cycle if branch occurs on same page, add 2 cycles if branch occurs to different page
    LUT[0xB0] = opcode<&AM_REL, &OP_BCS, 2, false>;         // Add 1 cycle if branch occurs on same page, add 2 cycles if branch occurs to different page
    LUT[0xF0] = opcode<&AM_REL, &OP_BEQ, 2, false>;         // Add 1 cycle if branch occurs on same page, add 2 cycles if branch occurs to different page
    LUT[0x24] = opcode<&AM_ZRO, &OP_BIT, 3, false>;
    LUT[0x2C] = opcode<&AM_ABS, &OP_BIT, 4, false>;
    LUT[0x30] = opcode<&AM_REL, &OP_BMI, 2, false>;         // Add 1 cycle if branch occurs on same page, add 2 cycles if branch occurs to different page
    LUT[0xD0] = opcode<&AM_REL, &OP_BNE, 2, false>;         // Add 1 cycle if branch occurs on same page, add 2 cycles if branch occurs to different page
    LUT[0x10] = opcode<&AM_REL, &OP_BPL, 2, false>;         // Add 1 cycle if branch occurs on same page, add 2 cycles if branch occurs to different page
    LUT[0x00] = opcode<&AM_IMP, &OP_BRK, 7, false>;
    LUT[0x50] = opcode<&AM_REL, &OP_BVC, 2, false>;         // Add 1 cycle if branch occurs on same page, add 2 cycles if branch occurs to different page
    LUT[0x70] = opcode<&AM_REL, &OP_BVS, 2, false>;         // Add 1 cycle if branch occurs on same page, add 2 cycles if branch occurs to different page
    LUT[0x18] = opcode<&AM_IMP, &OP_CLC, 2, false>;
    LUT[0xD8] = opcode<&AM_IMP, &OP_CLD, 2, false>;
    LUT[0x58] = opcode<&AM_IMP, &OP_CLI, 2, false>;
    LUT[0xB8] = opcode<&AM_IMP, &OP_CLV, 2, false>;
    LUT[0xC9] = opcode<&AM_IMM, &OP_CMP, 2, false>;
    LUT[0xC5] = opcode<&AM_ZRO, &OP_CMP, 3, false>;
    LUT[0xD5] = opcode<&AM_ZRX, &OP_CMP, 4, false>;
    LUT[0xCD] = opcode<&AM_ABS, &OP_CMP, 4, false>;
    LUT[0xDD] = opcode<&AM_INX, &OP_CMP, 4, true>;          // Add 1 to cycles if page boundary is crossed
    LUT[0xD9] = opcode<&AM_INY, &OP_CMP, 4, true>;          // Add 1 to cycles if page boundary is crossed
    LUT[0xC1] = opcode<&AM_INDX, &OP_CMP, 6, false>;
    LUT[0xD1] = opcode<&AM_INDY, &OP_CMP, 5, true>;         // Add 1 to cycles if page boundary is crossed
    LUT[0xE0] = opcode<&AM_IMM, &OP_CPX, 2, false>;
    LUT[0xE4] = opcode<&AM_ZRO, &OP_CPX, 3, false>;
    LUT[0xEC] = opcode<&AM_ABS, &OP_CPX, 4, false>;
    LUT[0xC0] = opcode<&AM_IMM, &OP_CPY, 2, false>;
    LUT[0xC4] = opcode<&AM_ZRO, &OP_CPY, 3, false>;
    LUT[0xCC] = opcode<&AM_ABS, &OP_CPY, 4, false>;
    LUT[0xC6] = opcode<&AM_ZRO, &OP_DEC, 5, false>;
    LUT[0xD6] = opcode<&AM_ZRX, &OP_DEC, 6, false>;
    LUT[0xCE] = opcode<&AM_ABS, &OP_DEC, 6, false>;
    LUT[0xDE] = opcode<&AM_INX, &OP_DEC, 7, false>;
    LUT[0xCA] = opcode<&AM_IMP, &OP_DEX, 2, false>;
    LUT[0x88] = opcode<&AM_IMP, &OP_DEY, 2, false>;
    LUT[0x49] = opcode<&AM_IMM, &OP_EOR, 2, false>;
    LUT[0x45] = opcode<&AM_ZRO, &OP_EOR, 3, false>;
    LUT[0x55] = opcode<&AM_ZRX, &OP_EOR, 4, false>;
    LUT[0x4D] = opcode<&AM_ABS, &OP_EOR, 4, false>;
    LUT[0x5D] = opcode<&AM_INX, &OP_EOR, 4, true>;          // Add 1 to cycles if page boundary is crossed
    LUT[0x59] = opcode<&AM_INY, &OP_EOR, 4, true>;          // Add 1 to cycles if page boundary is crossed
    LUT[0x41] = opcode<&AM_INDX, &OP_EOR, 6, false>;
    LUT[0x51] = opcode<&AM_INDY, &OP_EOR, 5, true>;         // Add 1 to cycles if page boundary is crossed
    LUT[0xE6] = opcode<&AM_ZRO, &OP_INC, 5, false>;
    LUT[0xF6] = opcode<&AM_ZRX, &OP_INC, 6, false>;
    LUT[0xEE] = opcode<&AM_ABS, &OP_INC, 6, false>;
    LUT[0xFE] = opcode<&AM_INX, &OP_INC, 7, false>;
    LUT[0xE8] = opcode<&AM_IMP, &OP_INX, 2, false>;
    LUT[0xC8] = opcode<&AM_IMP, &OP_INY, 2, false>;
    LUT[0x4C] = opcode<&AM_ABS, &OP_JMP, 3, false>;
    LUT[0x6C] = opcode<&AM_IND, &OP_JMP, 5, false>;
    LUT[0x20] = opcode<&AM_ABS, &OP_JSR, 6, false>;
    LUT[0xA9] = opcode<&AM_IMM, &OP_LDA, 2, false>;
    LUT[0xA5] = opcode<&AM_ZRO, &OP_LDA, 3, false>;
    LUT[0xB5] = opcode<&AM_ZRX, &OP_LDA, 4, false>;
    LUT[0xAD] = opcode<&AM_ABS, &OP_LDA, 4, false>;
    LUT[0xBD] = opcode<&AM_INX, &OP_LDA, 4, true>;          // Add 1 to cycles if page boundary is crossed
    LUT[0xB9] = opcode<&AM_INY, &OP_LDA, 4, true>;          // Add 1 to cycles if page boundary is crossed
    LUT[0xA1] = opcode<&AM_INDX, &OP_LDA, 6, false>;
    LUT[0xB1] = opcode<&AM_INDY, &OP_LDA, 5, true>;         // Add 1 to cycles if page boundary is crossed
    LUT[0xA2] = opcode<&AM_IMM, &OP_LDX, 2, false>;
    LUT[0xA6] = opcode<&AM_ZRO, &OP_LDX, 3, false>;
    LUT[0xB6] = opcode<&AM_ZRY, &OP_LDX, 4, false>;
    LUT[0xAE] = opcode<&AM_ABS, &OP_LDX, 4, false>;
    LUT[0xBE] = opcode<&AM_INY, &OP_LDX, 4, true>;          // Add 1 to cycles if page boundary is crossed
    LUT[0xA0] = opcode<&AM_IMM, &OP_LDY, 2, false>;
    LUT[0xA4] = opcode<&AM_ZRO, &OP_LDY, 3, false>;
    LUT[0xB4] = opcode<&AM_ZRX, &OP_LDY, 4, false>;
    LUT[0xAC] = opcode<&AM_ABS, &OP_LDY, 4, false>;
    LUT[0xBC] = opcode<&AM_INX, &OP_LDY, 4, true>;          // Add 1 to cycles if page boundary is crossed
    LUT[0x4A] = opcode<&AM_IMP, &OP_LSR, 2, false>;
    LUT[0x46] = opcode<&AM_ZRO, &OP_LSR, 5, false>;
    LUT[0x56] = opcode<&AM_ZRX, &OP_LSR, 6, false>;
    LUT[0x4E] = opcode<&AM_ABS, &OP_LSR, 6, false>;
    LUT[0x5E] = opcode<&AM_INX, &OP_LSR, 7, false>;
    LUT[0xEA] = opcode<&AM_IMP, &OP_NOP, 2, false>;
    LUT[0x09] = opcode<&AM_IMM, &OP_ORA, 2, false>;
    LUT[0x05] = opcode<&AM_ZRO, &OP_ORA, 3, false>;
    LUT[0x15] = opcode<&AM_ZRX, &OP_ORA, 4, false>;
    LUT[0x0D] = opcode<&AM_ABS, &OP_ORA, 4, false>;
    LUT[0x1D] = opcode<&AM_INX, &OP_ORA, 4, true>;          // Add 1 to cycles if page boundary is crossed
    LUT[0x19] = opcode<&AM_INY, &OP_ORA, 4, true>;          // Add 1 to cycles if page boundary is crossed
    LUT[0x01] = opcode<&AM_INDX, &OP_ORA, 6, false>;
    LUT[0x11] = opcode<&AM_INDY, &OP_ORA, 5, true>;         // Add 1 to cycles if page boundary is crossed
    LUT[0x48] = opcode<&AM_IMP, &OP_PHA, 3, false>;
    LUT[0x08] = opcode<&AM_IMP, &OP_PHP, 3, false>;
    LUT[0x68] = opcode<&AM_IMP, &OP_PLA, 4, false>;
    LUT[0x28] = opcode<&AM_IMP, &OP_PLP, 4, false>;
    LUT[0x2A] = opcode<&AM_IMP, &OP_ROL, 2, false>;
    LUT[0x26] = opcode<&AM_ZRO, &OP_ROL, 5, false>;
    LUT[0x36] = opcode<&AM_ZRX, &OP_ROL, 6, false>;
    LUT[0x2E] = opcode<&AM_ABS, &OP_ROL, 6, false>;
    LUT[0x3E] = opcode<&AM_INX, &OP_ROL, 7, false>;
    LUT[0x6A] = opcode<&AM_IMP, &OP_ROR, 2, false>;
    LUT[0x66] = opcode<&AM_ZRO, &OP_ROR, 5, false>;
    LUT[0x76] = opcode<&AM_ZRX, &OP_ROR, 6, false>;
    LUT[0x6E] = opcode<&AM_ABS, &OP_ROR, 6, false>;
    LUT[0x7E] = opcode<&AM_INX, &OP_ROR, 7, false>;
    LUT[0x40] = opcode<&AM_IMP, &OP_RTI, 6, false>;
    LUT[0x60] = opcode<&AM_IMP, &OP_RTS, 6, false>;
    LUT[0xE9] = opcode<&AM_IMM, &OP_SBC, 2, false>;
    LUT[0xE5] = opcode<&AM_ZRO, &OP_SBC, 3, false>;
    LUT[0xF5] = opcode<&AM_ZRX, &OP_SBC, 4, false>;
    LUT[0xED] = opcode<&AM_ABS, &OP_SBC, 4, false>;
    LUT[0xFD] = opcode<&AM_INX, &OP_SBC, 4, true>;          // Add 1 to cycles if page boundary is crossed
    LUT[0xF9] = opcode<&AM_INY, &OP_SBC, 4, true>;          // Add 1 to cycles if page boundary is crossed
    LUT[0xE1] = opcode<&AM_INDX, &OP_SBC, 6, false>;
    LUT[0xF1] = opcode<&AM_INDY, &OP_SBC, 5, true>;         // Add 1 to cycles if page boundary is crossed
    LUT[0x38] = opcode<&AM_IMP, &OP_SEC, 2, false>;
    LUT[0xF8] = opcode<&AM_IMP, &OP_SED, 2, false>;
    LUT[0x78] = opcode<&AM_IMP, &OP_SEI, 2, false>;
    LUT[0x85] = opcode<&AM_ZRO, &OP_STA, 3, false>;
    LUT[0x95] = opcode<&AM_ZRX, &OP_STA, 4, false>;
    LUT[0x8D] = opcode<&AM_ABS, &OP_STA, 4, false>;
    LUT[0x9D] = opcode<&AM_INX, &OP_STA, 5, false>;
    LUT[0x99] = opcode<&AM_INY, &OP_STA, 5, false>;
    LUT[0x81] = opcode<&AM_INDX, &OP_STA, 6, false>;
    LUT[0x91] = opcode<&AM_INDY, &OP_STA, 6, false>;
    LUT[0x86] = opcode<&AM_ZRO, &OP_STX, 3, false>;
    LUT[0x96] = opcode<&AM_ZRY, &OP_STX, 4, false>;
    LUT[0x8E] = opcode<&AM_ABS, &OP_STX, 4, false>;
    LUT[0x84] = opcode<&AM_ZRO, &OP_STY, 3, false>;
    LUT[0x94] = opcode<&AM_ZRX, &OP_STY, 4, false>;
    LUT[0x8C] = opcode<&AM_ABS, &OP_STY, 4, false>;
    LUT[0xAA] = opcode<&AM_IMP, &OP_TAX, 2, false>;
    LUT[0xA8] = opcode<&AM_IMP, &OP_TAY, 2, false>;
    LUT[0xBA] = opcode<&AM_IMP, &OP_TSX, 2, false>;
    LUT[0x8A] = opcode<&AM_IMP, &OP_TXA, 2, false>;
    LUT[0x9A] = opcode<&AM_IMP, &OP_TXS, 2, false>;
    LUT[0x98] = opcode<&AM_IMP, &OP_TYA, 2, false>;
}

int tick6507() {
    arg = 0x00;
    uint16_t operatorAddr = PC;
    uint8_t Operator = GETMEM8(PC);
    currentOpcode = Operator;

    if (LUT[Operator] == nullptr) {
        if (!CPUTEST && DEBUG && !DUMP) {
            std::cout << "Opcode not found: 0x" << getHex(Operator) << std::endl;
        }
        return -1;
    }
    LUT[Operator]();

    if (DEBUG) {
        // I'm too scared to use GETMEM8 here, as it might trigger
        // a strobe or a bankswitch incorrectly or something

        opCall newCall(operatorAddr, MEM[arg], Operator);
        callStack.push_back(newCall);
    }		

    steps++;
    return 0;
}

template<void (*adressingModePtr)(), void(*functionPtr)(), int cycleAmount, bool OPCycleException>
void opcode() {
    cycleException = OPCycleException;
    (*adressingModePtr)();  //Simply sets the adressing mode
    getArg();               //Sets 'arg' based on previous AM function
    (*functionPtr)();       //Calls operator
    cycle6507(cycleAmount);
}

// The argument is set to be a pointer to the final argument in memory
// This is so that address can also be written to in opcode-function
// Not just read from in 'getArg' and then passed as a temporary read-only value.
void getArg() {
    switch(AddrMode) {
        case IMP:   //1 Byte
            // Implied Adressing means single-byte instructions
            // It is also referred to accumulator adressing in some cases
            // Depending on the function of the opcode 
            // Therefore needs to be handled in-function in those cases
            PC++;
            break;
        case IMM:   //2 Bytes
            arg = PC+1;                   //Get PC (PC), serve as ptr to arg
            PC+=2;
            break;
        case ABS:   //3 Bytes
            //Adresses are little-endian [10], [30]  -> 3010)
            arg = GETMEM16(PC+1);         //Get 2b arg (GETMEM16(PC)), serve as ptr
            PC+=3;
            break;
        case ZRO:   //2 Bytes
            arg = GETMEM8(PC+1);          //Get 1b arg (MEM[PC]), serve as ptr
            PC+=2;
            break;
        case ZRX:   //2 Bytes
            // Wraps into zero page
            arg = (GETMEM8(PC+1)+X)&0xFF;     //Get 1b arg (MEM[PC]) offset X (+X), serve as ptr
            PC+=2;
            break;
        case ZRY:   //2 Bytes
            // Wraps into zero page
            arg = (GETMEM8(PC+1)+Y)&0xFF;            //Get 1b arg (MEM[PC]) offset Y (+X), serve as ptr
            PC+=2;
            break;
        case INX: {  //3 Bytes
            uint16_t noIndXAddr = GETMEM16(PC + 1);
            arg = noIndXAddr + X;       //Get 2b arg (GETMEM16(PC)) offset X (+X), serve as ptr
            if (cycleException && getPage(arg) != getPage(noIndXAddr)) {
                cycle6507(1);
            }
            PC += 3;
            break;
        }
        case INY: {   //3 Bytes
            uint16_t noIndYAddr = GETMEM16(PC + 1);
            arg = noIndYAddr + Y;       //Get 2b arg (GETMEM16(PC)) offset Y (+Y), serve as ptr
            if (cycleException && getPage(arg) != getPage(noIndYAddr)) {
                cycle6507(1);
            }
            PC += 3;

            break;
        }
        case IND:   //3 Bytes
            // Wraps into zero page
            // Differs from ABS in that it points to a value of 2 bytes, which can not be implemented
            // here, as the AM merely sets the arg to a pointer to the value
            // Only used in JMP, requires in-function handling because of 2 byte return val
            arg = GETMEM16(PC+1);                   //Get 2b arg (GETMEM16(PC)), serve as ptr to 2b value
            PC+=3;
            break;
        case INDX: {  //2 Bytes
            // 16 bit mem read has to be done in INDX & INDY
            // This is because GETMEM16 uses index loc and loc+1
            // So even when the input param loc is masked with 0xFF
            // loc + 1 will overflow out of zero page which should not
            // happen in INDX and INDY
            uint8_t loc = GETMEM8(PC + 1) + X;
            arg = (((uint16_t)GETMEM8((loc + 1) & 0xFF) << 8) | ((uint16_t)GETMEM8(loc & 0xFF)));
            // Get 1b arg (MEM[PC]), offset X (+X), get 2b val at loc, serve as ptr
            PC += 2;
        }
            break;
        case INDY: {  //2 Bytes
            // See INDX for explanation  
            uint8_t loc = GETMEM8(PC + 1);
            uint16_t noIndYAddr = (((uint16_t)GETMEM8((loc + 1) & 0xFF) << 8) | ((uint16_t)GETMEM8(loc & 0xFF)));
            arg = noIndYAddr + Y;
            // Get 1b arg (MEM[PC]), get 2b val at loc, offset Y (+Y), serve as ptr
            if (cycleException && getPage(arg) != getPage(noIndYAddr)) {
                cycle6507(1);
            }
            PC += 2;
        }
            break;
        case REL:   //2 Bytes
            // Relative addressing is the same as immediate addressing
            // The only difference being that the PC gets added to the
            // Argument (for branch instructions), to save space and 
            // Removing the need for 16 bit addresses in some cases.
            // Handled in-function
            arg = PC+1;                   //Get PC (PC), serve as ptr to arg
            PC+=2;
            break;
    }
}

void cycle6507(int cycleCount) {
    cycles += cycleCount;
}

//Get 16 bit (2 byte) value from memory at loc in little-endian
uint16_t GETMEM16(uint16_t loc) {
    return ((uint16_t)GETMEM8(loc+1)<<8) | ((uint16_t)GETMEM8(loc));
}

uint8_t GETMEM8(uint16_t loc) {
    if (CPUTEST) {
        return MEM[loc];
    }
     // Address decoder:
     /*
     Address           Destination
     ???1XXXX XXXXXXXX Cartridge
     ???0??0? 1XXXXXXX RAM
     ???0???? 0?XXXXXX TIA
     ???0??1? 1XXXXXXX RIOT
     */

    uint16_t readAddress;

    // Cartridge (???1XXXX XXXXXXXX):
    if ((loc & 0b0001000000000000) == 0b0001000000000000) {
        readAddress = loc & 0x1FFF;
        return cartRead(readAddress);
    }
    // PIA RAM (???0??0? 1XXXXXXX)
    if ((loc & 0b0001001010000000) == 0b0000000010000000) {
        readAddress = loc & 0xFF;
        return MEM[readAddress];
    }
    // TIA (???0???? 0?XXXXXX) "The reads seem to ignore A4 and A5."
    if ((loc & 0b0001000010000000) == 0) {
        readAddress = loc & 0b0000000000111111;
        return TIARead(readAddress);
    }
    // PIA IO (???0??1? 1XXXXXXX)
    if ((loc & 0b0001001010000000) == 0b0000001010000000) {
        readAddress = loc & 0b0000001011111111;
        return PIAread(readAddress);
    }
    // 6502 uses 16 address pins, 6507 uses 13
    readAddress = loc & 0x1FFF;
    return MEM[readAddress];
}

uint16_t GETRAM16(uint8_t loc) {
    return ((uint16_t)GETRAM8(loc+1)<<8)| ((uint16_t)GETRAM8(loc));
}

uint8_t GETRAM8(uint8_t loc) {
    return GETMEM8(loc+RAMoffset);        // Bit 7 of RAM addresses not used
}


void SETMEM16(uint16_t loc, uint16_t val) {
    SETMEM8(loc, val);
    SETMEM8(loc - 1, val>>8);
}

void SETMEM8(uint16_t loc, uint8_t val) {
    if (CPUTEST) {
        MEM[loc] = val;
        return;
    }

     // Address decoder:
     /*
     Address           Destination
     ???1XXXX XXXXXXXX Cartridge
     ???0??0? 1XXXXXXX RAM
     ???0???? 0?XXXXXX TIA
     ???0??1? 1XXXXXXX RIOT
     */

    uint16_t writeAddress;

    // Cartridge (???1XXXX XXXXXXXX):
    if ((loc & 0b0001000000000000) == 0b0001000000000000) {
        writeAddress = loc & 0x1FFF;
        cartWrite(writeAddress, val);
        return;
    }
    // PIA RAM (???0??0? 1XXXXXXX)
    if ((loc & 0b0001001010000000) == 0b0000000010000000) {
        writeAddress = loc & 0xFF;
        MEM[writeAddress] = val;
        return;
    }
    // TIA (???0???? 0?XXXXXX)
    if ((loc & 0b0001000010000000) == 0) {
        writeAddress = loc & 0b0000000000111111;
        TIAWrite(writeAddress, val);
        return;
    }
    // PIA IO (???0??1? 1XXXXXXX)
    if ((loc & 0b0001001010000000) == 0b0000001010000000) {
        writeAddress = loc & 0b0000001011111111;
        PIAwrite(writeAddress, val);
        return;
    }
    // 6502 uses 16 address pins, 6507 uses 13
    writeAddress = loc & 0x1FFF;
    MEM[writeAddress] = val;
}

void SETRAM16(uint8_t loc, uint16_t val) {
    SETRAM8(loc, val>>8);
    SETRAM8(loc - 1, val);
}

void SETRAM8(uint8_t loc, uint8_t val) {
    SETMEM8(loc+RAMoffset, val);
}


int16_t getPage(uint16_t address) {
    return address / 0x100;
}

uint8_t reverseByte(uint8_t inpByte) {
    uint8_t reversedByte = 0;
    for (int i = 0; i < 8; i++) {
        reversedByte = reversedByte | ((((inpByte & (0b1 << i))) >> i) << (7 - i));
    }
    return reversedByte;
}