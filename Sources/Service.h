#pragma once
#ifndef _SERVICE_DOT_H_
#define _SERVICE_DOT_H_

#include "Chip8.h"
#include "Renderer.h"

extern bool requiresRender;

class Service
{
	Chip8* emulatorEngine;
	Renderer* videoRenderer;

public:
	Service(Chip8*, Renderer*);
	void startROM(const char*, int);
};

#endif
