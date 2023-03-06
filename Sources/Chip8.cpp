#include "Chip8.h"
#include <fstream>
#include <random>
#include <windows.h>
#include <stdlib.h>

bool requiresRender = false;

Chip8::Chip8() {
	this->programCounter = START_ADDRESS;
	for (int i = 0; i < FONTSET_SIZE; i++)
		this->memory[FONTSET_START_ADDRESS + i] = this->fontset[i];
	srand(time(NULL));
	this->stackPointer = -1;
}

Chip8::~Chip8() {

}

void Chip8::loadROM(const char* filename) {
	std::ifstream fileIn(filename, std::ios::binary | std::ios::ate);
	if (fileIn.is_open()) {
		std::streampos fileSize = fileIn.tellg();
		char* fileBuffer = new char[fileSize];

		fileIn.seekg(0, std::ios::beg);
		fileIn.read(fileBuffer, fileSize);
		fileIn.close();

		for (int i = 0; i < fileSize; i++)
			this->memory[START_ADDRESS + i] = fileBuffer[i];

		delete[] fileBuffer;
	}
}

void Chip8::memoryDump() {
	printf("\n\n\nEngine RAM Dump\n");
	for (int i = 0; i < 128; i++) {
		printf("%4x: ", i * 32);
		for (int j = 0; j < 32; j++) {
			printf("%2x ", this->memory[i * 32 + j]);
		}
		printf("\n");
	}
	printf("\n");
}

void const* Chip8::getVideoData() {
	return this->videoMemory;
}

uint8_t* Chip8::getKeyData() {
	return this->keypad;
}

void Chip8::decrementTimers() {
	if(this->delayTimer > 0)
		this->delayTimer--;
	if (this->soundTimer > 0) {
		this->soundTimer--;
	}
}

void Chip8::cycle() {
	this->fetchedOpcode = (this->memory[this->programCounter] << 8u) | (this->memory[this->programCounter+1]);
	this->programCounter += 2;

	int decoder = (this->fetchedOpcode & 0xF000) >> 12u;
	(this->*opcodeDecodeTable[decoder])();
}

void Chip8::OP_0NNN() {
	// 0NNN - Jump to machine language code, not needed
}

void Chip8::OP_00E0() {
	// 00E0 - Clear Screen
	requiresRender = true;
	memset(this->videoMemory, 0, sizeof(this->videoMemory));
}

void Chip8::OP_00EE() {
	// OOEE - Ret
	this->programCounter = this->stack[this->stackPointer];
	this->stackPointer--;
}

void Chip8::opcodeDecode0() {
	if ((this->fetchedOpcode & 0x0FFF) == 0x00EE)
		this->OP_00EE();
	else if ((this->fetchedOpcode & 0x0FFF) == 0x00E0)
		this->OP_00E0();
	else
		this->OP_0NNN();
}

void Chip8::OP_1NNN() {
	// 1NNN - Jump
	this->programCounter = (this->fetchedOpcode & 0x0FFFu);
}

void Chip8::OP_2NNN() {
	// 2NNN - Call
	this->stackPointer++;
	this->stack[this->stackPointer] = this->programCounter;
	this->programCounter = this->fetchedOpcode & 0x0FFFu;
}

void Chip8::OP_3XNN() {
	// 3XNN - Skip one instruction if vX == NN
	uint8_t reg = (this->fetchedOpcode & 0x0F00u) >> 8u;
	uint8_t number = this->fetchedOpcode & 0x00FFu;
	if (this->registers[reg] == number)
		this->programCounter += 2;
}

void Chip8::OP_4XNN() {
	// 4XNN - Skip one instruction if vX != NN
	uint8_t reg = (this->fetchedOpcode & 0x0F00u) >> 8u;
	uint8_t number = this->fetchedOpcode & 0x00FFu;
	if (this->registers[reg] != number)
		this->programCounter += 2;
}

void Chip8::OP_5XY0() {
	// 5XY0 - Skip one instruction if vX == vY
	uint8_t regx = (this->fetchedOpcode & 0x0F00u) >> 8u;
	uint8_t regy = (this->fetchedOpcode & 0x00F0u) >> 4u;
	if (this->registers[regx] == this->registers[regy])
		this->programCounter += 2;
}

void Chip8::OP_9XY0() {
	// 9XY0 - Skip one instruction if vX != vY
	uint8_t regx = (this->fetchedOpcode & 0x0F00u) >> 8u;
	uint8_t regy = (this->fetchedOpcode & 0x00F0u) >> 4u;
	if (this->registers[regx] != this->registers[regy])
		this->programCounter += 2;
}

void Chip8::OP_6XNN() {
	// 6XNN - Set register vX to value NN
	uint8_t reg = ((this->fetchedOpcode & 0x0F00u) >> 8u);
	uint8_t value = ((this->fetchedOpcode & 0x00FFu));
	this->registers[reg] = value;
}

void Chip8::OP_7XNN() {
	// 7XNN - Add to register vX value NN
	uint8_t reg = ((this->fetchedOpcode & 0x0F00u) >> 8u);
	uint8_t value = ((this->fetchedOpcode & 0x00FFu));
	this->registers[reg] += value;
}

void Chip8::OP_8XY0() {
	// 8XY0 - Set vX to vY
	uint8_t regx = (this->fetchedOpcode & 0x0F00u) >> 8u;
	uint8_t regy = (this->fetchedOpcode & 0x00F0u) >> 4u;
	this->registers[regx] = this->registers[regy];
}

void Chip8::OP_8XY1() {
	// 8XY1 - vX = vX | vY
	uint8_t regx = (this->fetchedOpcode & 0x0F00u) >> 8u;
	uint8_t regy = (this->fetchedOpcode & 0x00F0u) >> 4u;
	this->registers[regx] = (this->registers[regx] | this->registers[regy]);
}

void Chip8::OP_8XY2() {
	// 8XY2 - vX = vX & vY
	uint8_t regx = (this->fetchedOpcode & 0x0F00u) >> 8u;
	uint8_t regy = (this->fetchedOpcode & 0x00F0u) >> 4u;
	this->registers[regx] = (this->registers[regx] & this->registers[regy]);
}

void Chip8::OP_8XY3() {
	// 8XY3 - vX = vX ^ vY
	uint8_t regx = (this->fetchedOpcode & 0x0F00u) >> 8u;
	uint8_t regy = (this->fetchedOpcode & 0x00F0u) >> 4u;
	this->registers[regx] = (this->registers[regx] ^ this->registers[regy]);
}

void Chip8::OP_8XY4() {
	// 8XY4 - Add vY to vX with overflow flag
	uint8_t regx = (this->fetchedOpcode & 0x0F00u) >> 8u;
	uint8_t regy = (this->fetchedOpcode & 0x00F0u) >> 4u;
	uint8_t oldvx = this->registers[regx];
	this->registers[regx] += this->registers[regy];
	if (this->registers[regx] < oldvx)
		this->registers[15] = 1;
	else
		this->registers[15] = 0;
}

void Chip8::OP_8XY5() {
	// OP_8XY5 - Sub vY from vX with overflow
	uint8_t regx = (this->fetchedOpcode & 0x0F00u) >> 8u;
	uint8_t regy = (this->fetchedOpcode & 0x00F0u) >> 4u;
	uint8_t oldvx = this->registers[regx];
	this->registers[regx] -= this->registers[regy];
	if (oldvx > this->registers[regy])
		this->registers[15] = 1;
	else
		this->registers[15] = 0;
}

void Chip8::OP_8XY6() {
	// OP_8XY6 - Shift right
	uint8_t regx = (this->fetchedOpcode & 0x0F00u) >> 8u;
	uint8_t regy = (this->fetchedOpcode & 0x00F0u) >> 4u;
	this->registers[regx] = this->registers[regy];
	uint8_t shiftedBit = this->registers[regx] & 0x0001u;
	this->registers[15] = shiftedBit;
	this->registers[regx] >>= 1;
}

void Chip8::OP_8XY7() {
	// OP_8XYy - vX = vY - vX with overflow
	uint8_t regx = (this->fetchedOpcode & 0x0F00u) >> 8u;
	uint8_t regy = (this->fetchedOpcode & 0x00F0u) >> 4u;
	uint8_t oldvx = this->registers[regx];
	this->registers[regx] = this->registers[regy] - this->registers[regx];
	if (this->registers[regy] > oldvx)
		this->registers[15] = 1;
	else
		this->registers[15] = 0;

}

void Chip8::OP_8XYE() {
	// OP_8XY6 - Shift left
	uint8_t regx = (this->fetchedOpcode & 0x0F00u) >> 8u;
	uint8_t regy = (this->fetchedOpcode & 0x00F0u) >> 4u;
	this->registers[regx] = this->registers[regy];
	uint8_t shiftedBit = this->registers[regx] & 0x8000u;
	this->registers[15] = shiftedBit;
	this->registers[regx] <<= 1;
}

void Chip8::opcodeDecode8() {
	uint8_t index = (this->fetchedOpcode & 0x000F);
	if (index == 14)
		index = 8;
	(this->*opcodeDecodeTable8[index])();
}

void Chip8::OP_ANNN() {
	// ANNN - Set index register to NNN
	uint16_t value = (this->fetchedOpcode & 0x0FFFu);
	this->indexRegister = value;
}

void Chip8::OP_BNNN() {
	// BNNN - Jump to address NNN + v0
	uint16_t address = (this->fetchedOpcode & 0x0FFFu);
	address += this->registers[0];
	this->programCounter = address;
}

void Chip8::OP_CXNN() {
	// CXNN - Random. Generates random & NN => vX
	uint8_t random = rand() % 256;
	random &= (this->fetchedOpcode & 0x00FFu);
	uint8_t reg = (this->fetchedOpcode & 0x0F00u) >> 8u;
	this->registers[reg] = random;
}

void Chip8::OP_DXYN() {
	// DXYN - Draw to screen at vX vY sprite size N
	requiresRender = true;
	uint8_t regX = (this->fetchedOpcode & 0x0F00u) >> 8u;
	uint8_t regY = (this->fetchedOpcode & 0x00F0u) >> 4u;

	uint8_t horizontalX = this->registers[regX] & 63;
	uint8_t verticalY = this->registers[regY] & 31;
	this->registers[15] = 0;
	uint8_t rows = this->fetchedOpcode & 0x000Fu;

	for (int i = 0; i < rows; i++) {
		uint8_t byteData = this->memory[this->indexRegister + i];
		for (int bite = 0; bite < 8; bite++) {
			uint8_t pixelFromByte = byteData & (0x80 >> bite);
			uint32_t* actualPixel = &this->videoMemory[(verticalY + i) * 64 + horizontalX + bite];

			if (pixelFromByte) {
				if (*actualPixel == 0xFFFFFFFF)
					this->registers[15] = 1;
				*actualPixel ^= 0xFFFFFFFF;
			}
		}
	}
}

void Chip8::OP_EX9E() {
	// EX93 - Skip one instruction if key value vX is pressed
	uint8_t regx = (this->fetchedOpcode & 0x0F00u) >> 8u;
	if (this->keypad[this->registers[regx]] == 1)
		this->programCounter += 2;
}

void Chip8::OP_EXA1() {
	// EX93 - Skip one instruction if key value vX is NOT pressed
	uint8_t regx = (this->fetchedOpcode & 0x0F00u) >> 8u;
	if (this->keypad[this->registers[regx]] == 0)
		this->programCounter += 2;
}

void Chip8::opcodeDecodeE() {
	if ((this->fetchedOpcode & 0x000F) == 0x000E)
		this->OP_EX9E();
	if ((this->fetchedOpcode & 0x000F) == 0x0001)
		this->OP_EXA1();
}

void Chip8::OP_FX07() {
	// FX07 sets VX to the current value of the delay timer
	uint8_t regx = (this->fetchedOpcode & 0x0F00u) >> 8u;
	this->registers[regx] = this->delayTimer;
}

void Chip8::OP_FX15() {
	// FX15 sets the delay timer to the value in VX
	uint8_t regx = (this->fetchedOpcode & 0x0F00u) >> 8u;
	this->delayTimer = this->registers[regx];
}

void Chip8::OP_FX18() {
	// FX18 sets the sound timer to the value in VX
	uint8_t regx = (this->fetchedOpcode & 0x0F00u) >> 8u;
	this->soundTimer = this->registers[regx];
}

void Chip8::OP_FX1E() {
	// FX1E The index register I will get the value in VX added to it.
	uint8_t regx = (this->fetchedOpcode & 0x0F00u) >> 8u;
	this->indexRegister += this->registers[regx];
	if (this->indexRegister > 0x1000u)
		this->registers[15] = 1;
}

void Chip8::OP_FX0A() {
	// FX0A Wait for key press and store in vX
	uint8_t regx = (this->fetchedOpcode & 0x0F00u) >> 8u;
	bool press = false;
	for(int i = 0; i < 16; i++)
		if (this->keypad[i] == 1) {
			this->registers[regx] = i;
			press = true;
			i = 16;
		}
	if (!press)
		this->programCounter -= 2;
}

void Chip8::OP_FX29() {
	// The index register I is set to the address of the hexadecimal character in VX
	uint8_t regx = (this->fetchedOpcode & 0x0F00u) >> 8u;
	uint8_t character = this->registers[regx] & 0x000F;
	this->indexRegister = FONTSET_START_ADDRESS + character * 5;
}

void Chip8::OP_FX33() {
	// This instruction is a little involved. 
	// It takes the number in VX (which is one byte, so it can be any number from 0 to 255) and converts it to three decimal digits, 
	// storing these digits in memory at the address in the index register I
	uint8_t regx = (this->fetchedOpcode & 0x0F00u) >> 8u;
	uint8_t number = this->registers[regx];
	this->memory[this->indexRegister + 2] = number % 10;
	number /= 10;
	this->memory[this->indexRegister + 1] = number % 10;
	number /= 10;
	this->memory[this->indexRegister] = number % 10;
}

void Chip8::OP_FX55() {
	// For FX55, the value of each variable register from V0 to VX inclusive (if X is 0, then only V0)
	uint8_t regLimit = (this->fetchedOpcode & 0x0F00u) >> 8u;
	uint16_t location = this->indexRegister;
	for (int i = 0; i <= regLimit; i++)
		this->memory[location + i] = this->registers[i];
}

void Chip8::OP_FX65() {
	uint8_t regLimit = (this->fetchedOpcode & 0x0F00u) >> 8u;
	uint16_t location = this->indexRegister;
	for (int i = 0; i <= regLimit; i++)
		this->registers[i] = this->memory[location + i];
}

void Chip8::opcodeDecodeF() {
	if ((this->fetchedOpcode & 0xF0FF) == 0xF007u)
		this->OP_FX07();
	if ((this->fetchedOpcode & 0xF0FF) == 0xF015u)
		this->OP_FX15();
	if ((this->fetchedOpcode & 0xF0FF) == 0xF018u)
		this->OP_FX18();
	if ((this->fetchedOpcode & 0xF0FF) == 0xF01Eu)
		this->OP_FX1E();
	if ((this->fetchedOpcode & 0xF0FF) == 0xF00Au)
		this->OP_FX0A();
	if ((this->fetchedOpcode & 0xF0FF) == 0xF029u)
		this->OP_FX29();
	if ((this->fetchedOpcode & 0xF0FF) == 0xF033u)
		this->OP_FX33();
	if ((this->fetchedOpcode & 0xF0FF) == 0xF055u)
		this->OP_FX55();
	if ((this->fetchedOpcode & 0xF0FF) == 0xF065u)
		this->OP_FX65();
}