#pragma once
#ifndef _RENDERER_DOT_H_
#define _RENDERER_DOT_H_

#include <SDL.h>

class Renderer
{
	SDL_Window* window;
	SDL_Surface* screenSurface;
	SDL_Renderer* renderer;
	SDL_Texture* texture;
	char const*  title;
	int screenWidth;
	int screenHeight;
	int scaledWidth;
	int scaledHeight;
	int pixelScaleFactor;

public:
	Renderer(int, int, int);				// Constructor
	~Renderer();
	void setupRenderer();				    // Setup phase
	bool readInput(uint8_t*);
	void renderScreen(const void*, int);
};

#endif

