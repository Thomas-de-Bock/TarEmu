#ifndef EMUAUDIO_H
#define EMUAUDIO_H

#include <SFML/Audio.hpp>
#include <iostream>
#include <string>
#include "6507.h"

extern sf::SoundBuffer* audioBuffer0;
extern sf::SoundBuffer* audioBuffer1;
extern sf::Sound* audio0;
extern sf::Sound* audio1;
extern uint8_t currentNoise0;
extern uint8_t currentNoise1;
extern uint8_t currentVol0;
extern uint8_t currentVol1;
extern uint8_t currentFreq0;
extern uint8_t currentFreq1;

extern float volumeMultiplier;
extern std::string appPath;



void initAudio();
void tickAudio();

void updateNoises(bool forced);
void updateVolumes(bool forced);
void updateFrequencies(bool forced);
void endAudio();



#endif
