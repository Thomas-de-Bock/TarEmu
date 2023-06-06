#include "PIA.h"

uint16_t INTIMclocks;
uint16_t currentInterval;
uint16_t normalInterval;

int totalTicks = 0;
bool f3Pressed = false;

void initPIA() {
	currentInterval = 0x400;
	INTIMclocks = currentInterval;
	normalInterval = currentInterval;
	MEM[0x284] = 0xD5;	// Randomly picked value
	if (DUMP) {
		MEM[0x284] = 0xE3;
	}

	/*
	  Bit        Expl.
	  SWCHB.0    Reset Button          (0=Pressed)
	  SWCHB.1    Select Button         (0=Pressed)
	  SWCHB.2    Not used
	  SWCHB.3    Color Switch          (0=B/W, 1=Color) (Always 0 for SECAM)
	  SWCHB.4-5  Not used
	  SWCHB.6    P0 Difficulty Switch  (0=Beginner (B), 1=Advanced (A))
	  SWCHB.7    P1 Difficulty Switch  (0=Beginner (B), 1=Advanced (A))
	*/
	SETMEM8(0x282, 0b00001011);
}

void updateInput() {
	//-----------------
	// SWCHB CONTROL  
	//-----------------

	// SWCHB.6    P0 Difficulty Switch  (0=Beginner (B), 1=Advanced (A))

	// P0 Advanced
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::F5)) {
		SETMEM8(0x282, GETMEM8(0x282) | 0b01000000);
	}
	// P1 Advanced
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::F7)) {
		SETMEM8(0x282, GETMEM8(0x282) | 0b10000000);
	}

	// P0 Beginner
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::F6)) {
		SETMEM8(0x282, GETMEM8(0x282) & 0b10111111);
	}
	// P1 Beginner
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::F8)) {
		SETMEM8(0x282, GETMEM8(0x282) & 0b01111111);
	}
	// Select button
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::F1)) {
		SETMEM8(0x282, GETMEM8(0x282) & 0b11111101);
	}
	else {
		SETMEM8(0x282, GETMEM8(0x282) | 0b00000010);
	}
	// Reset button
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::F2)) {
		SETMEM8(0x282, GETMEM8(0x282) & 0b11111110);
	}
	else {
		SETMEM8(0x282, GETMEM8(0x282) | 0b00000001);
	}
	// Switch color (BW / color)
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::F3)) {
		if (!f3Pressed) {
			SETMEM8(0x282, GETMEM8(0x282) ^ 0b00001000);
			f3Pressed = true;
		}
	}
	else {
		f3Pressed = false;
	}


	/*
	  Pin  PlayerP0  PlayerP1  Expl.
	  1    SWCHA.4   SWCHA.0   Up     (0=Moved, 1=Not moved)
	  2    SWCHA.5   SWCHA.1   Down   ("")
	  3    SWCHA.6   SWCHA.2   Left   ("")
	  4    SWCHA.7   SWCHA.3   Right  ("")
	  6    INPT4.7   INPT5.7   Button (0=Pressed, 1=Not pressed)
	*/
	// ! YOU CAN SET BITS TO INPUT IDK HOW IT WORKS THO
	SETMEM8(0x280, 0xFF);	// Reset SWCHA
	
	// Player 0
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
		SETMEM8(0x280, GETMEM8(0x280) & 0b11111110);
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
		SETMEM8(0x280, GETMEM8(0x280) & 0b11111101);
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
		SETMEM8(0x280, GETMEM8(0x280) & 0b11111011);
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
		SETMEM8(0x280, GETMEM8(0x280) & 0b11110111);
	}
	
	// Player 1
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
		SETMEM8(0x280, GETMEM8(0x280) & 0b11101111);
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
		SETMEM8(0x280, GETMEM8(0x280) & 0b11011111);
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
		SETMEM8(0x280, GETMEM8(0x280) & 0b10111111);
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
		SETMEM8(0x280, GETMEM8(0x280) & 0b01111111);
	}

	// This has to be done directly as it is read only
	MEM[0x3C] |= 0b10000000;	// Reset INPT4
	MEM[0x3D] |= 0b10000000;	// Reset INPT5

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
		MEM[0x3C] &= 0b01111111;
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::M)) {
		MEM[0x3D] &= 0b01111111;
	}


	// SWBCNT
	SETMEM8(0x282, (~GETMEM8(0x283)) & GETMEM8(0x282));

	// SWACNT
	SETMEM8(0x280, (~GETMEM8(0x281)) & GETMEM8(0x280));
}

void PIAwrite(uint16_t loc, uint8_t val) {
	
	if (loc == 0x294) {
		currentInterval = 0x01;
		normalInterval = 0x01;
		INTIMclocks = 0x01;
		MEM[0x284] = val;
		MEM[0x285] &= 0b01111111;	//INSTAT bit 7 reset
	}
	else if (loc == 0x295) {
		currentInterval = 0x08;
		normalInterval = 0x08;
		INTIMclocks = 0x08;
		MEM[0x284] = val;
		MEM[0x285] &= 0b01111111;	//INSTAT bit 7 reset
	}
	else if (loc == 0x296) {
		currentInterval = 0x40;
		normalInterval = 0x40;
		INTIMclocks = 0x40;
		MEM[0x284] = val;
		MEM[0x285] &= 0b01111111;	//INSTAT bit 7 reset
	}
	else if (loc == 0x297) {
		currentInterval = 0x400;
		normalInterval = 0x400;
		INTIMclocks = 0x400;
		MEM[0x284] = val;
		MEM[0x285] &= 0b01111111;	//INSTAT bit 7 reset
	}
	INTIMclocks--;	// "The timer is decremented once immediately after writing (ie. value 00h does immediately underflow)"
	
	MEM[loc] = val;
}

uint8_t PIAread(uint16_t loc) {
	// "The interval is automatically re-actived when reading from the INTIM register, ie. the timer does then continue to decrement at interval speed (originated at the current value)"
	if (loc == 0x284) {	// If read from INTIM
		currentInterval = normalInterval;
	}
	else if (loc == 0x285) {
		uint8_t INSTAT = MEM[0x285];
		MEM[0x285] &= 0b10111111;	//INSTAT bit 7 reset
		return INSTAT;
	}
	return MEM[loc];
}

void tickPIA() {	
	totalTicks++;	// Again, this has no reason to be here, this + the TIA are just the only components that tick every, well, tick
	if (INTIMclocks - 1 == 0) {			// When clock reaches 0
		INTIMclocks = currentInterval+(INTIMclocks-1);	// Set this back to interval
		uint8_t INTIMval = MEM[0x284];	// Did 1 interval, so 01 from the INTIMval
		
		if (INTIMval-1 < 0) {				// if, because of the decrement, INTIMval has now reached < 0
			currentInterval = 1;		// It makes it so the interval=machine cycle
			INTIMclocks = 1;			// INTIMclocks is just the counter for the constant interval
			MEM[0x284] = 0xFF;			// INTIM is reset
		}
		else {
			MEM[0x284]--;	// dec INTIM
		}
	}
	else {
		INTIMclocks--;
	}
	
}