#pragma once
#ifndef _CHIP_EIGHT_DOT_H_
#define _CHIP_EIGHT_DOT_H_

#include <stdint.h>

const int START_ADDRESS = 0x200;
const unsigned int FONTSET_SIZE = 80;
const int FONTSET_START_ADDRESS = 0x50;

class Chip8
{
	uint8_t registers[16]{};
	uint8_t memory[4096]{};
	uint32_t videoMemory[100 * 50]{};
	uint16_t programCounter{};
	uint16_t indexRegister{};
	uint16_t stack[64]{};
	uint16_t stackPointer{};
	uint8_t delayTimer{};
	uint8_t soundTimer{};
	uint16_t fetchedOpcode{};
	uint8_t keypad[16]{};
	uint8_t fontset[FONTSET_SIZE] = {
		0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
		0x20, 0x60, 0x20, 0x20, 0x70, // 1
		0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
		0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
		0x90, 0x90, 0xF0, 0x10, 0x10, // 4
		0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
		0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
		0xF0, 0x10, 0x20, 0x40, 0x40, // 7
		0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
		0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
		0xF0, 0x90, 0xF0, 0x90, 0x90, // A
		0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
		0xF0, 0x80, 0x80, 0x80, 0xF0, // C
		0xE0, 0x90, 0x90, 0x90, 0xE0, // D
		0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
		0xF0, 0x80, 0xF0, 0x80, 0x80  // F
	};

public:
	Chip8();
	~Chip8();
	void loadROM(const char*);
	void memoryDump();
	void cycle();

	void decrementTimers();

	void const* getVideoData();
	uint8_t* getKeyData();

	void OP_00E0();
	void OP_00EE();
	void OP_0NNN();

	void OP_1NNN();

	void OP_2NNN();

	void OP_3XNN();

	void OP_4XNN();

	void OP_5XY0();

	void OP_6XNN();

	void OP_7XNN();

	void OP_8XY0();
	void OP_8XY1();
	void OP_8XY2();
	void OP_8XY3();
	void OP_8XY4();
	void OP_8XY5();
	void OP_8XY6();
	void OP_8XY7();
	void OP_8XYE();

	void OP_9XY0();

	void OP_ANNN();

	void OP_BNNN();

	void OP_CXNN();

	void OP_DXYN();

	void OP_EX9E();
	void OP_EXA1();

	void OP_FX07();
	void OP_FX15();
	void OP_FX18();
	void OP_FX1E();
	void OP_FX0A();
	void OP_FX29();
	void OP_FX33();
	void OP_FX55();
	void OP_FX65();

	void opcodeDecode0();
	void opcodeDecode8();
	void opcodeDecodeE();
	void opcodeDecodeF();

	typedef void (Chip8:: *Chip8Func)();
	Chip8Func opcodeDecodeTable[16] = {&Chip8::opcodeDecode0, &Chip8::OP_1NNN, &Chip8::OP_2NNN, &Chip8::OP_3XNN,       &Chip8::OP_4XNN,
												  &Chip8::OP_5XY0, &Chip8::OP_6XNN, &Chip8::OP_7XNN, &Chip8::opcodeDecode8, &Chip8::OP_9XY0,
												  &Chip8::OP_ANNN, &Chip8::OP_BNNN, &Chip8::OP_CXNN, &Chip8::OP_DXYN, &Chip8::opcodeDecodeE,
												  &Chip8::opcodeDecodeF};
	Chip8Func opcodeDecodeTable8[9] = {&Chip8::OP_8XY0, &Chip8::OP_8XY1, &Chip8::OP_8XY2, &Chip8::OP_8XY3, &Chip8::OP_8XY4, &Chip8::OP_8XY5, 
								  &Chip8::OP_8XY6, &Chip8::OP_8XY7, &Chip8::OP_8XYE};
};

#endif

