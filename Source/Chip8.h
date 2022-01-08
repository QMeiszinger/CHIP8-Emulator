#ifndef CHIP8_H
#define CHIP8_H

#include <cstdint>
#include <cstring>

const uint16_t START_ADDRESS = 0x200;
const uint16_t FONTSET_START_ADDRESS = 0x50;
const uint16_t VIDEO_WIDTH = 64;
const uint16_t VIDEO_HEIGHT = 32;

class Chip8
{
public:

	Chip8();

private:

	//Fontset
	const uint8_t fontset[80] =
	{
		0xF0, 0x90, 0x90, 0x90, 0xF0, //0
		0x20, 0x60, 0x20, 0x20, 0x70, //1
		0xF0, 0x10, 0xF0, 0x80, 0xF0, //2
		0xF0, 0x10, 0xF0, 0x10, 0xF0, //3
		0x90, 0x90, 0xF0, 0x10, 0x10, //4
		0xF0, 0x80, 0xF0, 0x10, 0xF0, //5
		0xF0, 0x80, 0xF0, 0x90, 0xF0, //6
		0xF0, 0x10, 0x20, 0x40, 0x40, //7
		0xF0, 0x90, 0xF0, 0x90, 0xF0, //8
		0xF0, 0x90, 0xF0, 0x10, 0xF0, //9
		0xF0, 0x90, 0xF0, 0x90, 0x90, //A
		0xE0, 0x90, 0xE0, 0x90, 0xE0, //B
		0xF0, 0x80, 0x80, 0x80, 0xF0, //C
		0xE0, 0x90, 0x90, 0x90, 0xE0, //D
		0xF0, 0x80, 0xF0, 0x80, 0xF0, //E
		0xF0, 0x80, 0xF0, 0x80, 0x80  //F
	};

	//Chip8's 16 8-bit registers
	uint8_t registers[16] = { 0 };

	//Chip8's 4096 bytes of memory
	uint8_t memory[4096] = { 0 };

	//Index register
	uint16_t indexRegister = 0x0000;
	
	//Program counter
	uint16_t programCounter = 0x0000;

	//16 level stack
	uint16_t stack[16] = { 0 };

	//8-bit stack pointer
	uint8_t stackPointer = 0x00;

	//8-bit delay timer
	uint8_t delayTimer = 0x00;

	//8-bit sound timer
	uint8_t soundTimer = 0x00;

	//Variable for holding the opcode
	uint16_t opcode = 0x0000;

	typedef void (Chip8::* Chip8Func)();
	Chip8Func table[0xF + 1]{ &Chip8::OP_NULL };
	Chip8Func table0[0xE + 1]{ &Chip8::OP_NULL };
	Chip8Func table8[0xE + 1]{ &Chip8::OP_NULL };
	Chip8Func tableE[0xE + 1]{ &Chip8::OP_NULL };
	Chip8Func tableF[0x65 + 1]{ &Chip8::OP_NULL };


public:

	//Array of pixels for the screen
	uint32_t display[64 * 32] = { 0 };

	//Keypad
	uint8_t keypad[16] = { 0 };

	//Main cycle function
	void cycle();
	
	//Load ROM into memory
	void loadRom(const char* filename);

	//Table functions
	void Table0();
	void Table8();
	void TableE();
	void TableF();

	//Opcode instructions
	void OP_00E0();
	void OP_00EE();
	void OP_1XXX();
	void OP_2XXX();
	void OP_3XYY();
	void OP_4XYY();
	void OP_5XY0();
	void OP_6XYY();
	void OP_7XYY();
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
	void OP_AXXX();
	void OP_BXXX();
	void OP_CXYY();
	void OP_DXYN();
	void OP_EX9E();
	void OP_EXA1();
	void OP_FX07();
	void OP_FX0A();
	void OP_FX15();
	void OP_FX18();
	void OP_FX1E();
	void OP_FX29();
	void OP_FX33();
	void OP_FX55();
	void OP_FX65();
	void OP_NULL();

};

#endif
