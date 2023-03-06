#include "Service.h"
#include <chrono>

extern bool requiresRender;

Service::Service(Chip8* t_engine, Renderer* t_videoRenderer) {
	this->emulatorEngine = t_engine;
	this->videoRenderer = t_videoRenderer;
}

void Service::startROM(const char* filename, int cycleDelay) {
	this->emulatorEngine->loadROM(filename);
	this->emulatorEngine->memoryDump();
	this->videoRenderer->setupRenderer();

	bool quit = false;
	auto lastCycleTime = std::chrono::high_resolution_clock::now();
	uint64_t instructionCount = 0;

	while (!quit) {
		quit = this->videoRenderer->readInput(this->emulatorEngine->getKeyData());

		auto currentCycleTime = std::chrono::high_resolution_clock::now();
		float elapsedTime = std::chrono::duration<float, std::chrono::milliseconds::period>(currentCycleTime - lastCycleTime).count();

		if (elapsedTime > cycleDelay) {
			instructionCount += 1;
			if (instructionCount >= (1000 / (cycleDelay * 60))) {
				instructionCount = 0;
				this->emulatorEngine->decrementTimers();
			}
			lastCycleTime = currentCycleTime;
			this->emulatorEngine->cycle();
			if (requiresRender) {
				this->videoRenderer->renderScreen(this->emulatorEngine->getVideoData(), sizeof(uint32_t) * 64);
				requiresRender = false;
			}
		}
	}
}