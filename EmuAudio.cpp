#include "EmuAudio.h"

sf::SoundBuffer* audioBuffer0;
sf::SoundBuffer* audioBuffer1;
sf::Sound* audio0;
sf::Sound* audio1;
uint8_t currentNoise0;
uint8_t currentNoise1;
uint8_t currentVol0;
uint8_t currentVol1;
uint8_t currentFreq0;
uint8_t currentFreq1;

float volumeMultiplier = 10.0f;
std::string appPath;

void initAudio() {
	audioBuffer0 = new sf::SoundBuffer();
	audioBuffer1 = new sf::SoundBuffer();
	audio0 = new sf::Sound();
	audio1 = new sf::Sound();


	currentNoise0 = 0;
	currentNoise1 = 0;
	currentVol0 = 0;
	currentVol1 = 0;
	currentFreq0 = 0;
	currentFreq1 = 0;
	updateNoises(true);
	updateVolumes(true);
	updateFrequencies(true);
	audio0->setLoop(true);
	audio1->setLoop(true);
}

void tickAudio() {
	updateNoises(false);
	updateVolumes(false);
	updateFrequencies(false);
}

void updateNoises(bool forced) {
	std::string noisePath = appPath + "noises/0.wav";
	if (currentNoise0 != MEM[0x15] || forced) {
		//noisePath += std::to_string(MEM[0x15]);
		audioBuffer0->loadFromFile(noisePath);
		audio0->setBuffer(*audioBuffer0);
		currentNoise0 = MEM[0x15];
		audio0->play();
	}
	if (currentNoise1 != MEM[0x16] || forced) {
		//noisePath += std::to_string(MEM[0x16]);
		audioBuffer1->loadFromFile(noisePath);
		audio1->setBuffer(*audioBuffer1);
		currentNoise1 = MEM[0x16];
		audio1->play();
	}
}
void updateVolumes(bool forced) {
	if (currentVol0 != MEM[0x19] || forced) {
		audio0->setVolume(MEM[0x19] * volumeMultiplier);
		currentVol0 = MEM[0x19];
	}
	if (currentVol1 != MEM[0x1A] || forced) {
		audio1->setVolume(MEM[0x1A] * volumeMultiplier);
		currentVol1 = MEM[0x1A];
	}
}
void updateFrequencies(bool forced) {
	if (currentFreq0 != MEM[0x17] || forced) {
		audio0->setPitch(5.0f/(MEM[0x17]+1));
		currentFreq0 = MEM[0x17];
	}
	if (currentFreq1 != MEM[0x18] || forced) {
		audio1->setPitch(5.0f/(MEM[0x18]+1));
		currentFreq1 = MEM[0x18];
	}
}

void endAudio() {
	free(audioBuffer0);
	free(audioBuffer1);
	free(audio0);
	free(audio1);
}
