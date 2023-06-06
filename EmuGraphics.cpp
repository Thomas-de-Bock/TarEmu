#include "EmuGraphics.h"


sf::RenderWindow EmuGraphics::window;
std::shared_ptr<sf::Uint8[]> EmuGraphics::screen;
sf::Texture EmuGraphics::screenTexture;
sf::Sprite* EmuGraphics::screenSprite;
bool EmuGraphics::cropScreen = true;
std::chrono::steady_clock::time_point EmuGraphics::frameStartTime;

void EmuGraphics::initScreen() {
    //Man do i not know how smartptrs work
    std::shared_ptr<sf::Uint8[]> newScreen(new sf::Uint8[CRT_HSCREEN * CRT_VSCREEN * 4]());
    screen = newScreen;

    // Approximately close to actual stretch
    window.create(sf::VideoMode((CRT_HSCREEN - CRT_HBLANK)*5, (CRT_VSCREEN - CRT_VBLANK - CRT_VSYNC - CRT_OVERSCAN) * 3), "Atari 2600");
    screenTexture.create(CRT_HSCREEN, CRT_VSCREEN);
    window.setActive(false);
}

void EmuGraphics::loopScreen() {
    while (window.isOpen())
    {
        // check all the window's events that were triggered since the last iteration of the loop
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }
    }
}


void EmuGraphics::drawScreen() {
    using namespace std::chrono_literals;
    using namespace std::chrono;

    duration<double, std::nano> frameTime = duration_cast<duration<double>>(steady_clock::now() - frameStartTime);
    //std::cout << "FPS: " << 1000000000/frameTime.count() << std::endl;
    screenTexture.update(screen.get());
    screenSprite = new sf::Sprite(screenTexture);
    if (cropScreen) {
        // Values found in the Stella emulator, seem to give the best results on most games
        screenSprite->setTextureRect(sf::IntRect(CRT_HBLANK, 23, CRT_HSCREEN - CRT_HBLANK, 228));

        screenSprite->setScale(5, 2.5);
    }
    window.clear();
    window.draw(*screenSprite);
    window.display();
    delete screenSprite;
    frameStartTime = steady_clock::now();
}

void EmuGraphics::closeScreen() {

}