#include "TIA.h"


void (*TIASTROBEMEM[0x3E])();		// 0000-003D
uint8_t PMBPOS[5];
uint8_t GRPdelayed[2];
uint8_t ENABLdelayed;					// Used for vertical delay, transefered to actual ENABL when writing to GRP1
bool objectsDrawn[6];

uint8_t CRT_VBLANK = 37;
uint8_t CRT_HBLANK = 68;
uint8_t CRT_OVERSCAN = 30;
uint16_t CRT_VSCREEN = 262;	// very unfortunate amount
uint8_t CRT_HSCREEN = 228;
uint8_t CRT_VSYNC = 3;

uint16_t clockCount = 0;

uint8_t NTSCcolors[0xFF * 3];
int8_t HMtable[1248];

void (*bufferedRESxx)();
int16_t bufferedGRP[2];

void initTIA() {
	// Strobe functions
	TIASTROBEMEM[0x02] = TIA_WSYNC;
	TIASTROBEMEM[0x03] = TIA_RSYNC;

	TIASTROBEMEM[0x10] = TIA_RESP0_BUFFR;
	TIASTROBEMEM[0x11] = TIA_RESP1_BUFFR;
	TIASTROBEMEM[0x12] = TIA_RESM0_BUFFR;
	TIASTROBEMEM[0x13] = TIA_RESM1_BUFFR;
	TIASTROBEMEM[0x14] = TIA_RESBL_BUFFR;

	TIASTROBEMEM[0x2A] = TIA_HMOVE;
	TIASTROBEMEM[0x2B] = TIA_HMCLR;
	TIASTROBEMEM[0x2C] = TIA_CXCLR;

	initNTSC();
	initHMOVE();

	clockCount = CRT_VSYNC * CRT_HSCREEN;	// 3 scnline VSYNC does not occur at start obviously

	PMBPOS[0] = 2;
	PMBPOS[1] = 2;
	PMBPOS[2] = 1;
	PMBPOS[3] = 1;
	PMBPOS[4] = 1;

	GRPdelayed[0] = 0;
	GRPdelayed[1] = 0;

}

// Function needed since in my emulator, it first performs CPU tick
// Then lets the TIA catch up, while in reality it's more similar to the exact 
// Opposite, this messes with graphics in text and scores, and positioning
// See vars for better explanation
void doBufferedTIA() {
	if (bufferedRESxx != nullptr) {
		(*bufferedRESxx)();
		bufferedRESxx = nullptr;
		return;
	}
	if (bufferedGRP[0] != -1) {  	// if loc is GRP0, transfer GRP1 to GRP1 delayed
		GRPdelayed[1] = MEM[0x1C];	// Transfer GRP1 to GRP1 delayed

		MEM[0x1B] = (uint8_t)bufferedGRP[0];
		bufferedGRP[0] = -1;
		return;
	}
	if (bufferedGRP[1] != -1) {		// if loc is GRP1, transfer GRP0 to GRP0 delayed and ENABL to ENABL delayed
		GRPdelayed[0] = MEM[0x1B];	// Transfer GRP0 to GRP0 delayed
		ENABLdelayed = MEM[0x1F];	// Transfer ENABL to ENABL delayed

		MEM[0x1C] = (uint8_t)bufferedGRP[1];
		bufferedGRP[1] = -1;
		return;
	}
}

void initHMOVE() {
	// Relies on the fact that HM.table has a space at the end
	std::ifstream t(appPath + "tables/HM.table");
	std::stringstream buffer;
	buffer << t.rdbuf();
	std::string HMOVEoffsetStr = buffer.str();
	std::string currentVal = "";
	int currentHMoffset = 0;
	for (uint32_t i = 0; i < HMOVEoffsetStr.size(); i++) {
		if ((HMOVEoffsetStr[i] == ' ' || HMOVEoffsetStr[i] == '\n') && currentVal.size() != 0) {
			HMtable[currentHMoffset] = stoi(currentVal);
			currentVal = "";
			currentHMoffset++;
		}
		else if(HMOVEoffsetStr[i] != ' ' && HMOVEoffsetStr[i] != '\n') {
			currentVal += HMOVEoffsetStr[i];
		}
	}

}

void initNTSC() {
	// Relies on the fact that NTSCpalette.table has a space at the end
	std::ifstream t(appPath + "/tables/NTSCpalette.table");
	std::stringstream buffer;
	buffer << t.rdbuf();
	std::string ColorStr = buffer.str();
	std::string currentVal = "";
	int currentColorOffset = 0;

	for (uint32_t i = 0; i < ColorStr.size(); i++) {
		if ((ColorStr[i] == ' ' || ColorStr[i] == '\n') && currentVal.size() != 0) {
			NTSCcolors[currentColorOffset] = stoi(currentVal);
			currentVal = "";
			currentColorOffset++;
			if (currentColorOffset % 3 == 0 && currentColorOffset != 0) {
				currentColorOffset += 3;
			}
		}
		else if (ColorStr[i] != ' ' && ColorStr[i] != '\n') {
			currentVal += ColorStr[i];
		}
	}

	for (int i = 3; i < 0xFF*3; i+=6) {
		NTSCcolors[i] = NTSCcolors[i-3];
		NTSCcolors[i + 1] = NTSCcolors[i - 2];
		NTSCcolors[i + 2] = NTSCcolors[i - 1];
	}
}

// Addresses already wrapped around when calling this function,
// No further 13 pin/bit masking required
void TIAWrite(uint16_t loc, uint8_t val) {
	if (loc < 0x2D && TIASTROBEMEM[loc] != nullptr) {
		(*TIASTROBEMEM[loc])();
		return;
	}

	if (loc == 0x1B) {
		bufferedGRP[0] = val;	// Schedule write for correct time
		return;
	}

	if (loc == 0x1C) {
		bufferedGRP[1] = val;	// Schedule write for correct time
		return;
	}
	if (loc >= 0x30) {
		// Write to TIA read only
		MEM[loc] = val;
		return;
	}

	MEM[loc] = val;
}

uint8_t TIARead(uint16_t loc) {

	// Read from the TIA write onlies which are mapped to readable
	/*
	  0x30(CXM0P) = 0x00,
	  0x31(CXM1P) = 0x01,
	  0x32(CXP0FB) = 0x02,
	  0x33(CXP1FB) = 0x03,
	  0x34(CXM0FB) = 0x04,
	  0x35(CXM1FB) = 0x05,
	  0x36(CXBLPF) = 0x06,
	  0x37(CXPPMM) = 0x07,
	  0x38(INPT0) = 0x08,
	  0x39(INPT1) = 0x09,
	  0x3A(INPT2) = 0x0a,
	  0x3B(INPT3) = 0x0b,
	  0x3C(INPT4) = 0x0c,
	  0x3D(INPT5) = 0x0d
	*/
	loc = (loc & 0x0F) | 0x30;

	if (loc >= 0x3E) {
		// Not a valid TIA read
		return 0;
	}

	return MEM[loc];
}

void tickTIA() {
	if ((MEM[0x01] & 0b00000010) == 0b00000010) {	// Turn off electron beam if VBLANK bit 1 is set
		goto finishedDrawing;
	}

	if (clockCount > (CRT_HSCREEN * CRT_VSCREEN) - 1) {
		goto finishedDrawing;
	}
	RESMPx_TIA(0);
	RESMPx_TIA(1);
	/*
	  1 (highest)  COLUP0   P0, M0  (and left side of PF in SCORE-mode)
	  2            COLUP1   P1, M1  (and right side of PF in SCORE-mode)
	  3            COLUPF   BL, PF  (only BL in SCORE-mode)
	  4 (lowest)   COLUBK   BK

	if CTRPF.2:

	  1 (highest)  COLUPF   PF, BL  (always, the SCORE-bit is ignored)
	  2            COLUP0   P0, M0
	  3            COLUP1   P1, M1
	  4 (lowest)   COLUBK   BK

	*/
	if((MEM[0x0A] & 0b100) == 0) {	// if CTRPF bit 2 is set, change priorities
		drawBG_TIA();
		drawPF_TIA();
		drawBL_TIA();
		drawMx_TIA(1);
		drawPx_TIA(1);
		drawMx_TIA(0);
		drawPx_TIA(0);
	}
	else {
		drawBG_TIA();
		drawMx_TIA(1);
		drawPx_TIA(1);
		drawMx_TIA(0);
		drawPx_TIA(0);
		drawBL_TIA();
		drawPF_TIA();
	}
	setCollisions_TIA();


finishedDrawing:

	clockCount++;

	// if VSCYNC signal set and beam outside of VBLANK period,
	// set clock to top of screen, which has a 3 scanline VSYNC "border"
	// Apparently the length of this VSYNC time can vary but i'm sure it's cool dont worry abt it
	if (((MEM[0x00] & 0b00000010) == 0b00000010 && (getScanLine() > 37))) {
		// Only reset scanline not colorclock
		clockCount = clockCount-getVclock();
		EmuGraphics::drawScreen();
		stepFrameButton = false;
		for (uint32_t i = 0; i < CRT_HSCREEN * CRT_VSCREEN * 4; i++) {
			EmuGraphics::screen[i] = 0;
		}
		updateInput(); // Update input every frame
	}

	if (clockCount % CRT_HSCREEN == 0) {	// Start of HBLANK (new scanline)
		RDY = true;		// Restart CPU at start of each scanline
		if (stepLineButton) {
			EmuGraphics::drawScreen();
			stepLineButton = false;
		}
		return;
	}
}
void drawPF_TIA() {
	if (getColorClock() < CRT_HBLANK) {
		return;
	}
	uint8_t currentByte = 0;
	uint8_t currentBit = 0;
	bool Reflected = ((MEM[0x0A] & 0b00000001) == 0b1);	// Read CTRLPF register bit 0 (reflection)
	uint8_t Hclock = getColorClock() - CRT_HBLANK;
	bool isRightSide = false;
	if (Hclock >= (CRT_HSCREEN - CRT_HBLANK) / 2) {
		Hclock -= (CRT_HSCREEN - CRT_HBLANK) / 2;	//Wrap clock to left side
		isRightSide = true;
	}

	// There is 100% a way to to this mathematically in a single line, but I am too stupid
	if (Hclock < 4 * 4) { // 4 Bits each representing 4 pixels (clockCounts)
	// (68-71) (72-75) (76-79) and (80-83)

		if (isRightSide && Reflected) {
			currentByte = MEM[0x0F];
			currentBit = ((Hclock) / 4);	
		}
		else {
			currentByte = MEM[0x0D];	// PF0
			currentBit = ((Hclock) / 4) + 4; // Offset by 4, PF0 bit 0-4 is ignored	
		}
		// How many times you can divide it by 4 is the amount of bits it's passed, (integer divis is floored)
		// Because 1 bit = 4 pixels
	}
	else if (Hclock < 4 * 12) {
		if (isRightSide && Reflected) {
			currentByte = Hclock >= 4 * 8 ? reverseByte(MEM[0x0E]) : MEM[0x0F];
			currentBit = Hclock >= 4 * 8 ? ((Hclock - 4 * 8) / 4) : ((Hclock) / 4);
		}
		else {	
			currentByte = reverseByte(MEM[0x0E]); // PF1 is drawn in reverse order for some reason
			currentBit = ((Hclock - 4 * 4) / 4);
		}
	}
	else if (Hclock < 4 * 20) {
		if (isRightSide && Reflected) {
			currentByte = Hclock >= 4 * 16 ? MEM[0x0D] : reverseByte(MEM[0x0E]);
			currentBit = Hclock >= 4 * 16 ? ((Hclock - 4 * 16) / 4) : ((Hclock - 4 * 8) / 4);
		}
		else {
			currentByte = MEM[0x0F];	// PF2
			currentBit = ((Hclock - 4 * 12) / 4);
		}
	}
	if (isRightSide && Reflected) {
		currentByte = reverseByte(currentByte);
	}

	if ((currentByte & (0b1<<currentBit)) != 0) { // Check if current bit is set
		uint8_t* PFcolor = getNTSCcolor(MEM[0x08]);
		uint8_t* P0color = getNTSCcolor(MEM[0x06]);
		uint8_t* P1color = getNTSCcolor(MEM[0x07]);
		// If D1 (score) is set, PF gets color of P0 for left half and P1 for right half
		uint8_t* currentColor = (MEM[0x0A] & 0b00000010) == 0b10 ? P0color : PFcolor;
		if (currentColor == P0color && isRightSide) {	// P1 color for right side
			currentColor = P1color;
		}

		objectsDrawn[5] = true;
		setScreenPixel(clockCount, currentColor);

		free(PFcolor);
		free(P0color);
		free(P1color);
	}

}
void drawBG_TIA() {
	// 09 is COLUBK 
	using namespace EmuGraphics;
	uint8_t* COLUBK = getNTSCcolor(MEM[0x09]);
	setScreenPixel(clockCount, COLUBK);
	free(COLUBK);
}

void drawMx_TIA(uint8_t Mx) {
	if ((MEM[0x1D + Mx] & 0b10) == 0) {	// Check ENAMx enable missile Graphic
		return;
	}

	if ((MEM[0x28 + Mx] & 0b10) == 0b10) {	// Check RESMPx, it sets Mx to Px and hide Mx
		return;
	}

	uint8_t Msize = (MEM[0x04 + Mx] & 0b00110000)>>4;	// Gets NUSIZx, bit 4-5 for Mx size
	Msize = 0b01 << Msize;									// 1, 2, 4, 8
	uint8_t Mpos = getGraphicPos(2 + Mx) + CRT_HBLANK;
	if (getColorClock() < Mpos || getColorClock() >= Mpos + Msize) {
		return;
	}
	objectsDrawn[2+Mx] = true;
	uint8_t* Mcol = getNTSCcolor(MEM[0x06 + Mx]);
	setScreenPixel(clockCount, Mcol);
	free(Mcol);
}

void drawBL_TIA() {
	uint8_t BLenabled = MEM[0x1F];

	if ((MEM[0x27] & 0b1) == 0b1) {	// if VDELBL is set, read from ENABL delayed instead
		BLenabled = ENABLdelayed;
	}

	if ((BLenabled & 0b10) == 0) {		// Check if ball is enabled
		return;
	}

	uint8_t BLsize = (MEM[0x0A] & 0b00110000) >> 4;			// Gets CTRLPF, bit 4-5 for Mx size
	BLsize = 0b01 << BLsize;									// 1, 2, 4, 8
	uint8_t BLpos = getGraphicPos(4) + CRT_HBLANK;
	if (getColorClock() < BLpos || getColorClock() >= BLpos + BLsize) {
		return;
	}

	objectsDrawn[4] = true;
	uint8_t* BLcol = getNTSCcolor(MEM[0x08]);
	setScreenPixel(clockCount, BLcol);
	free(BLcol);
}

void drawPx_TIA(uint8_t Px) {

	uint8_t GRPx = MEM[0x1B + Px];
	if ((MEM[0x25+Px] & 0b1) == 0b1) {	// if VDELPx is set, read from GRPx delayed instead
		GRPx = GRPdelayed[Px];
	}
	if (GRPx == 0) {
		return;
	}
	
	uint8_t Ppos = getGraphicPos(Px) + CRT_HBLANK;
	uint8_t Psize = 1;		// Size of player sprite
	uint8_t Pcops = 1;		// Amount of copies of player sprite
	uint8_t PcopDist = 0;	// Distance between copies of player sprite
	/*0  One copy              (X.........)
	  1  Two copies - close    (X.X.......)
	  2  Two copies - medium   (X...X.....)
	  3  Three copies - close  (X.X.X.....)
	  4  Two copies - wide     (X.......X.)
	  5  Double sized player   (XX........)
	  6  Three copies - medium (X...X...X.)
	  7  Quad sized player     (XXXX......) */

	switch (MEM[0x04 + Px]&0b00000111) {
		case 0:
			break;
		case 1: Pcops = 2; PcopDist = 1;
			break;
		case 2: Pcops = 2; PcopDist = 3;
			break;
		case 3: Pcops = 3; PcopDist = 1;
			break;
		case 4: Pcops = 2; PcopDist = 7;
			break;
		case 5: Psize = 2; 
			break;
		case 6: Pcops = 3; PcopDist = 3;
			break;
		case 7: Psize = 4;
			break;
	}
	uint8_t PcopNum = 0;	// Which copy the beam is currently on
	bool isOnPx = false;	// If is currently on a player pixel
	for (int i = 0; i < Pcops; i++) {
		if (getColorClock() >= Ppos+(PcopDist*8*i)+8*i && getColorClock() < Ppos + (PcopDist * 8 * i) + 8 * i + 8*Psize) {
			isOnPx = true;
			PcopNum = i;
			break;
		}
	}

	if (!isOnPx) {
		return;
	}
	bool Reflected = (MEM[0x0B+Px]&0b1000)==0b1000;	// bit 3 Reflect Player Graphics
	uint8_t Hclock = getColorClock();
	uint8_t currentBit = (Hclock - (Ppos + (PcopDist * PcopNum * 8) + 8 * PcopNum))/Psize;
	if (((GRPx & (0b1 << currentBit)) != 0  && Reflected) || ((reverseByte(GRPx) & (0b1 << currentBit)) != 0 && !Reflected)) {
		objectsDrawn[Px] = true;
		uint8_t* Pcol = getNTSCcolor(MEM[0x06 + Px]);
		setScreenPixel(clockCount, Pcol);
		free(Pcol);
	}
}

void setCollisions_TIA() { // Worst possible way to do this
	/*	30h - CXM0P (R) - Collision Latch M0-P1, M0-P0 (Bit 7,6) (Read only)
		31h - CXM1P (R) - Collision Latch M1-P0, M1-P1 (Bit 7,6) (Read only)
		32h - CXP0FB (R) - Collision Latch P0-PF, P0-BL (Bit 7,6) (Read only)
		33h - CXP1FB (R) - Collision Latch P1-PF, P1-BL (Bit 7,6) (Read only)
		34h - CXM0FB (R) - Collision Latch M0-PF, M0-BL (Bit 7,6) (Read only)
		35h - CXM1FB (R) - Collision Latch M1-PF, M1-BL (Bit 7,6) (Read only)
		36h - CXBLPF (R) - Collision Latch BL-PF (Bit 7) (Read only)
		37h - CXPPMM (R) - Collision Latch P0-P1, M0-M1 (Bit 7,6) (Read only)
		  Bit  Expl.
		  0-5  Not used
		  6,7  Two Collsion Flags   (0=No collision, 1=Collision occured) */
	// Implementation in order:
	if (objectsDrawn[2] && objectsDrawn[1]) { //M0-P1, bit 7
		MEM[0x30] |= 0b01000000;
	}
	if (objectsDrawn[2] && objectsDrawn[0]) { //M0-P0, bit 6
		MEM[0x30] |= 0b10000000;
	}
	if (objectsDrawn[3] && objectsDrawn[0]) { //M1-P0, bit 7
		MEM[0x31] |= 0b10000000;
	}
	if (objectsDrawn[3] && objectsDrawn[1]) { //M1-P1, bit 6
		MEM[0x31] |= 0b01000000;
	}
	if (objectsDrawn[0] && objectsDrawn[5]) { //P0-PF, bit 7
		MEM[0x32] |= 0b10000000;
	}
	if (objectsDrawn[0] && objectsDrawn[4]) { //P0-BL, bit 6
		MEM[0x32] |= 0b01000000;
	}
	if (objectsDrawn[1] && objectsDrawn[5]) { //P1-PF, bit 7
		MEM[0x33] |= 0b10000000;
	}
	if (objectsDrawn[1] && objectsDrawn[4]) { //P1-BL, bit 6
		MEM[0x33] |= 0b01000000;
	}
	if (objectsDrawn[2] && objectsDrawn[5]) { //M0-PF, bit 7
		MEM[0x34] |= 0b10000000;
	}
	if (objectsDrawn[2] && objectsDrawn[4]) { //M0-BL, bit 6
		MEM[0x34] |= 0b01000000;
	}
	if (objectsDrawn[3] && objectsDrawn[5]) { //M1-PF, bit 7
		MEM[0x35] |= 0b10000000;
	}
	if (objectsDrawn[3] && objectsDrawn[4]) { //M1-BL, bit 6
		MEM[0x35] |= 0b01000000;
	}
	if (objectsDrawn[4] && objectsDrawn[5]) { //BL-PF, bit 7
		MEM[0x36] |= 0b10000000;
	}
	if (objectsDrawn[0] && objectsDrawn[1]) { //P0-P1, bit 7
		MEM[0x37] |= 0b10000000;
	}
	if (objectsDrawn[2] && objectsDrawn[3]) { //M0-M1, bit 6
		MEM[0x37] |= 0b01000000;
	}
	memset(objectsDrawn, 0, 6 * sizeof(bool));
}

void RESMPx_TIA(uint8_t Px) {
	if ((MEM[0x28 + Px] & 0b10) == 0) {
		return;
	}
	uint8_t Poffset = 3;
	switch (MEM[0x04 + Px]) {	// Offset depends on player size
	case 5: Poffset = 6;
		break;
	case 7: Poffset = 10;
		break;
	}

	setGraphicPos(2 + Px, getGraphicPos(Px));
	changeGraphicPos(2 + Px, Poffset);
}

//=================== 
// Strobe functions =
//===================

void TIA_WSYNC() {
	RDY = false;
}
void TIA_RSYNC() {
	// Should be delayed, but barely used so it is what it is
	clockCount = getVclock() + 225;
}

// Noticed RES never set the positions to that exact position always some offset
// These are what I managed to find though they likely aren't actually constant
void TIA_RESP0() {
	if (getColorClock() < CRT_HBLANK) {
		setGraphicPos(0, 3);
		return;
	}
	setGraphicPos(0, getColorClock() - CRT_HBLANK + 5);
}
void TIA_RESP1() {
	if (getColorClock() < CRT_HBLANK) {
		setGraphicPos(1, 3);
		return;
	}
	setGraphicPos(1, getColorClock() - CRT_HBLANK + 5);
}
void TIA_RESM0() {
	if (getColorClock() < CRT_HBLANK) {
		setGraphicPos(2, 2);
		return;
	}
	setGraphicPos(2, getColorClock() - CRT_HBLANK + 4);
}
void TIA_RESM1() {
	if (getColorClock() < CRT_HBLANK) {
		setGraphicPos(3, 2);
		return;
	}
	setGraphicPos(3, getColorClock() - CRT_HBLANK + 4);
}
void TIA_RESBL() {
	if (getColorClock() < CRT_HBLANK) {
		setGraphicPos(4, 2);
		return;
	}
	setGraphicPos(4, getColorClock() - CRT_HBLANK + 4);
}

void TIA_RESP0_BUFFR() {
	bufferedRESxx = TIA_RESP0;
}
void TIA_RESP1_BUFFR() {
	bufferedRESxx = TIA_RESP1;
}
void TIA_RESM0_BUFFR() {
	bufferedRESxx = TIA_RESM0;
}
void TIA_RESM1_BUFFR() {
	bufferedRESxx = TIA_RESM1;
}
void TIA_RESBL_BUFFR() {
	bufferedRESxx = TIA_RESBL;
}

void TIA_HMOVE() { 
	changeGraphicPos(0, HMtoOffset(MEM[0x20]));
	changeGraphicPos(1, HMtoOffset(MEM[0x21]));
	changeGraphicPos(2, HMtoOffset(MEM[0x22]));
	changeGraphicPos(3, HMtoOffset(MEM[0x23]));
	changeGraphicPos(4, HMtoOffset(MEM[0x24]));
}

void TIA_HMCLR() {
	MEM[0x20] = 0;	//HM P0
	MEM[0x21] = 0;	//HM P1
	MEM[0x22] = 0;	//HM M0
	MEM[0x23] = 0;	//HM M1
	MEM[0x24] = 0;	//HM BL
}
void TIA_CXCLR() {
	memset(&MEM[0x30], 0, 8 * sizeof(uint8_t));
}

uint8_t* getNTSCcolor(uint8_t col) {
	uint8_t* returnColor = (uint8_t*)malloc(sizeof(uint8_t) * 3);
	returnColor[0] = NTSCcolors[col * 3];
	returnColor[1] = NTSCcolors[col * 3 + 1];
	returnColor[2] = NTSCcolors[col * 3 + 2];
	return returnColor;
}

void setScreenPixel(uint16_t loc, uint8_t* col) {
	using namespace EmuGraphics;
	screen.get()[loc * 4] = col[0];
	screen.get()[loc * 4+1] = col[1];
	screen.get()[loc * 4+2] = col[2];
	screen.get()[loc * 4+3] = 255;
}

int8_t HMtoOffset(uint8_t HMbyte) {
	/*
	     HMPx values
		 0   1   2   3   4   5   6   7   8   9   a   b   c   d   e   f     
	Cyc
	00   0  -1  -2  -3  -4  -5  -6  -7   8   7   6   5   4   3   2   1      
	01   0  -1  -2  -3  -4  -5  -6  -7   8   7   6   5   4   3   2   1      
	02   0  -1  -2  -3  -4  -5  -6  -7   8   7   6   5   4   3   2   1      
	
	19   5   5   5   5   5   5   5   5   8   7   6   5   5   5   5   5      
	20   6   6   6   6   6   6   6   6   8   7   6   6   6   6   6   6          
	..      
	54   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0   0     
	..
	73  -8  -9 -10 -11 -12 -13 -14 -15   0  -1  -2  -3  -4  -5  -6  -7
	74  -8  -9 -10 -11 -12 -13 -14 -15   0  -1  -2  -3  -4  -5  -6  -7   
	75   0  -1  -2  -3  -4  -5  -6  -7   8   7   6   5   4   3   2   1      
	.. etc

	Stored in file: "HM.table", loaded to HMtable var at TIA init
*/
	return HMtable[(getColorClock()/3)*16 + ((HMbyte&0xF0)>>4)];
}

uint16_t getColorClock() {
	return clockCount - (CRT_HSCREEN * (clockCount / CRT_HSCREEN));
}
uint16_t getVclock() {
	return CRT_HSCREEN * (clockCount / CRT_HSCREEN);	// This does not just return clockCount, integer division is floored
}

uint16_t getScanLine() {
	return (clockCount / CRT_HSCREEN);
}

uint8_t getGraphicPos(uint8_t PMB) {
	uint8_t wrappedPos = PMBPOS[PMB] - ((PMBPOS[PMB] / 160) * 160);
	return wrappedPos;
}

void setGraphicPos(uint8_t PMB, uint16_t newPos) {
	PMBPOS[PMB] = (int)newPos - (((int)newPos / 160) * 160);
}
void changeGraphicPos(uint8_t PMB, int16_t deltaPos) {
	int unwrappedPos = (int)PMBPOS[PMB] + (int)deltaPos;	// Pretty sure no cast is needed here but just to be sure

	PMBPOS[PMB] = unwrappedPos - ((unwrappedPos / 160) * 160);	// Int division is floored
}