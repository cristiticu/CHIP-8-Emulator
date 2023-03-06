#include "Renderer.h"
#include <stdio.h>

Renderer::Renderer(int width, int height, int scale) {
	this->window = nullptr;
	this->screenSurface = nullptr;
	this->renderer = nullptr;
	this->texture = nullptr;

	this->screenWidth = width;
	this->screenHeight = height;
	this->pixelScaleFactor = scale;

	this->scaledWidth = this->screenWidth * this->pixelScaleFactor;
	this->scaledHeight = this->screenHeight * this->pixelScaleFactor;

	this->title = "CHIP-8 Emulator @boss";
}

Renderer::~Renderer() {
	printf("Cleaning up\n");
	SDL_DestroyTexture(this->texture);
	SDL_DestroyRenderer(this->renderer);
	SDL_DestroyWindow(this->window);
	SDL_Quit();
	printf("Done. Exiting\n");
}

void Renderer::Renderer::setupRenderer() {
	printf("Initialising SDL Renderer\n");
	SDL_Init(SDL_INIT_VIDEO);
	this->window = SDL_CreateWindow(this->title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, this->scaledWidth, this->scaledHeight, SDL_WINDOW_SHOWN);
	this->renderer = SDL_CreateRenderer(this->window, -1, SDL_RENDERER_ACCELERATED);
	this->texture = SDL_CreateTexture(this->renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, this->screenWidth, this->screenHeight);
	printf("Renderer initialised\n");
}

void Renderer::renderScreen(void const* videoBuffer, int pitch) {
	SDL_UpdateTexture(this->texture, nullptr, videoBuffer, pitch);
	SDL_RenderClear(this->renderer);
	SDL_RenderCopy(this->renderer, this->texture, nullptr, nullptr);
	SDL_RenderPresent(this->renderer);
}

bool Renderer::readInput(uint8_t* keypad) {
	bool exit = false;
	SDL_Event inputEvent;
	while (SDL_PollEvent(&inputEvent)) {
		switch (inputEvent.type) {
		case SDL_QUIT:
			exit = true;
			break;
		case SDL_KEYDOWN:
			switch (inputEvent.key.keysym.sym) {
			case SDLK_ESCAPE:
				exit = true;
				break;
			case SDLK_x:
				keypad[0] = 1;
				break;
			case SDLK_1:
				keypad[1] = 1;
				break;
			case SDLK_2:
				keypad[2] = 1;
				break;
			case SDLK_3:
				keypad[3] = 1;
				break;
			case SDLK_q:
				keypad[4] = 1;
				break;
			case SDLK_w:
				keypad[5] = 1;
				break;
			case SDLK_e:
				keypad[6] = 1;
				break;
			case SDLK_a:
				keypad[7] = 1;
				break;
			case SDLK_s:
				keypad[8] = 1;
				break;
			case SDLK_d:
				keypad[9] = 1;
				break;
			case SDLK_z:
				keypad[10] = 1;
				break;
			case SDLK_c:
				keypad[11] = 1;
				break;
			case SDLK_4:
				keypad[12] = 1;
				break;
			case SDLK_r:
				keypad[13] = 1;
				break;
			case SDLK_f:
				keypad[14] = 1;
				break;
			case SDLK_v:
				keypad[15] = 1;
				break;
			}
			break;
		case SDL_KEYUP:
			switch (inputEvent.key.keysym.sym) {
			case SDLK_ESCAPE:
				exit = true;
				break;
			case SDLK_x:
				keypad[0] = 0;
				break;
			case SDLK_1:
				keypad[1] = 0;
				break;
			case SDLK_2:
				keypad[2] = 0;
				break;
			case SDLK_3:
				keypad[3] = 0;
				break;
			case SDLK_q:
				keypad[4] = 0;
				break;
			case SDLK_w:
				keypad[5] = 0;
				break;
			case SDLK_e:
				keypad[6] = 0;
				break;
			case SDLK_a:
				keypad[7] = 0;
				break;
			case SDLK_s:
				keypad[8] = 0;
				break;
			case SDLK_d:
				keypad[9] = 0;
				break;
			case SDLK_z:
				keypad[10] = 0;
				break;
			case SDLK_c:
				keypad[11] = 0;
				break;
			case SDLK_4:
				keypad[12] = 0;
				break;
			case SDLK_r:
				keypad[13] = 0;
				break;
			case SDLK_f:
				keypad[14] = 0;
				break;
			case SDLK_v:
				keypad[15] = 0;
				break;
			}
			break;
		}
	}
	return exit;
}