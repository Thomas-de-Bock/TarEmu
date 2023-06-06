#ifndef EMUGRAPHICS_H
#define EMUGRAPHICS_H

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include "TIA.h"
#include<chrono>

namespace EmuGraphics {
	
	extern sf::RenderWindow window;
	extern std::shared_ptr<sf::Uint8[]> screen;			// Current Screen, drawn when full, * 4 for RGBA channels
	extern sf::Texture screenTexture;
	extern sf::Sprite* screenSprite;
	extern bool cropScreen;
	//extern std::chrono::steady_clock::time_point frameStartTime;
	extern std::chrono::steady_clock::time_point frameStartTime;

	void initScreen();

	void loopScreen();

	void drawScreen();

	void closeScreen();
}

#endif