#include <stdio.h>
#include <iostream>
#include "Renderer.h"
#include "Service.h"
#include <string>
#include "Chip8.h"

#define DEBUG_MODE false
#define SCREEN_WIDTH 64
#define SCREEN_HEIGHT 32

int main(int argc, char* args[]) {
	int scaleFactor = 16;
	int cycleDelay = 3;

	printf("CHIP-8 Emulator\n");
	if ((argc <= 1 || argc > 4) && DEBUG_MODE == false) {
		std::cerr << "Wrong arguments\nuse: " << args[0] << "file    [delay]    [scale]\n";
		std::exit(EXIT_FAILURE);
	}
	if (argc == 3)
		cycleDelay = std::stoi(args[2]);
	if (argc == 4)
		scaleFactor = std::stoi(args[3]);

	Renderer mainVideoRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, scaleFactor);
	Chip8 mainEmulatorEngine;
	Service mainService(&mainEmulatorEngine, &mainVideoRenderer);

	if (DEBUG_MODE)
		mainService.startROM("Maze (alt) [David Winter, 199x].ch8", cycleDelay);
	else
		mainService.startROM(args[1], cycleDelay);
	return 0;
}
