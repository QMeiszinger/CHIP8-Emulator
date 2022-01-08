#include "Chip8.h"

#include <fstream>

Chip8::Chip8()
{

	//Set program counter to first executable instruction
	programCounter = START_ADDRESS;

	//Load fonts into memory
	for (int i = 0; i < 80; i++)
	{
		memory[FONTSET_START_ADDRESS + i] = fontset[i];
	}
	
	//========== CODE PROVIDED BY AUSTIN MORLAN: https://code.austinmorlan.com/explore/repos ==========
	// Set up function pointer table
	table[0x0] = &Chip8::Table0;
	table[0x1] = &Chip8::OP_1XXX;
	table[0x2] = &Chip8::OP_2XXX;
	table[0x3] = &Chip8::OP_3XYY;
	table[0x4] = &Chip8::OP_4XYY;
	table[0x5] = &Chip8::OP_5XY0;
	table[0x6] = &Chip8::OP_6XYY;
	table[0x7] = &Chip8::OP_7XYY;
	table[0x8] = &Chip8::Table8;
	table[0x9] = &Chip8::OP_9XY0;
	table[0xA] = &Chip8::OP_AXXX;
	table[0xB] = &Chip8::OP_BXXX;
	table[0xC] = &Chip8::OP_CXYY;
	table[0xD] = &Chip8::OP_DXYN;
	table[0xE] = &Chip8::TableE;
	table[0xF] = &Chip8::TableF;

	table0[0x0] = &Chip8::OP_00E0;
	table0[0xE] = &Chip8::OP_00EE;

	table8[0x0] = &Chip8::OP_8XY0;
	table8[0x1] = &Chip8::OP_8XY1;
	table8[0x2] = &Chip8::OP_8XY2;
	table8[0x3] = &Chip8::OP_8XY3;
	table8[0x4] = &Chip8::OP_8XY4;
	table8[0x5] = &Chip8::OP_8XY5;
	table8[0x6] = &Chip8::OP_8XY6;
	table8[0x7] = &Chip8::OP_8XY7;
	table8[0xE] = &Chip8::OP_8XYE;

	tableE[0x1] = &Chip8::OP_EXA1;
	tableE[0xE] = &Chip8::OP_EX9E;

	tableF[0x07] = &Chip8::OP_FX07;
	tableF[0x0A] = &Chip8::OP_FX0A;
	tableF[0x15] = &Chip8::OP_FX15;
	tableF[0x18] = &Chip8::OP_FX18;
	tableF[0x1E] = &Chip8::OP_FX1E;
	tableF[0x29] = &Chip8::OP_FX29;
	tableF[0x33] = &Chip8::OP_FX33;
	tableF[0x55] = &Chip8::OP_FX55;
	tableF[0x65] = &Chip8::OP_FX65;

}



void Chip8::cycle()
{

	//Fetch
	opcode = (memory[programCounter] << 8) | memory[programCounter + 1];

	//Increment program counter
	programCounter += 2;

	//Decode and execute
	((*this).*(table[(opcode & 0xF000) >> 12]))();

	// Decrement the delay timer if it's been set
	if (delayTimer > 0)
	{
		--delayTimer;
	}

	// Decrement the sound timer if it's been set
	if (soundTimer > 0)
	{
		--soundTimer;
	}

}

void Chip8::loadRom(const char* filename)
{

	//Open rom as a binary file and move the file pointer to the end of the file
	std::ifstream rom(filename, std::ios::binary | std::ios::ate);

	if (rom.is_open())
	{

		 //Get size of file and create a buffer
		 std::streampos size = rom.tellg();
		 char* buffer = new char[size];

		 //Seek to beggining of file and read into the buffer
		 rom.seekg(0, std::ios::beg);
		 rom.read(buffer, size);
		 rom.close();

		 //Load the contents from the buffer into the Chip8's memory
		 for (long i = 0; i < size; i++)
		 {
			 memory[START_ADDRESS + i] = buffer[i];
		 }

		 //Free memory allocated to buffer
		 delete[] buffer;

	}

}

void Chip8::Table0()
{
	((*this).*(table0[opcode & 0x000Fu]))();
}

void Chip8::Table8()
{
	((*this).*(table8[opcode & 0x000Fu]))();
}

void Chip8::TableE()
{
	((*this).*(tableE[opcode & 0x000Fu]))();
}

void Chip8::TableF()
{
	((*this).*(tableF[opcode & 0x00FFu]))();
}

//CLR: Clear the display
void Chip8::OP_00E0()
{
	memset(display, 0, sizeof(display));
}

//RET: Return from subroutine
void Chip8::OP_00EE()
{
	--stackPointer;
	programCounter = stack[stackPointer];
}

//JP addr: Jump to location XXX
void Chip8::OP_1XXX()
{
	uint16_t addr = opcode & 0x0FFF;
	programCounter = addr;
}

//CALL addr: Call subroutine at XXX
void Chip8::OP_2XXX()
{

	//Get address from opcode
	uint16_t addr = opcode & 0x0FFF;

	//Push current program counter to stack
	stack[stackPointer] = programCounter;
	++stackPointer;

	//Set address into program counter
	programCounter = addr;

}

//SE Vx, byte: Skip next instruction if Vx = YY
void Chip8::OP_3XYY()
{

	//Decode X and YY
	uint8_t Vx= (opcode & 0x0F00) >> 8;
	uint8_t YY = opcode & 0x00FF;

	if (registers[Vx] == YY)
	{
		programCounter += 2;
	}

}

//SNE Vx, byte: Skip next instruction if Vx != YY
void Chip8::OP_4XYY()
{

	//Decode X and YY
	uint8_t Vx = (opcode & 0x0F00) >> 8;
	uint8_t YY = opcode & 0x00FF;

	if (registers[Vx] != YY)
	{
		programCounter += 2;
	}

}

//SE Vx, Vy: Skip next instruction if Vx = Vy
void Chip8::OP_5XY0()
{

	//Decode X and Y
	uint8_t Vx = (opcode & 0x0F00) >> 8;
	uint8_t Vy = (opcode & 0x00F0) >> 4;

	if (registers[Vx] == registers[Vy])
	{
		programCounter += 2;
	}

}

//LD Vx, byte: Set Vx = YY
void Chip8::OP_6XYY()
{

	//Decode X and YY
	uint8_t Vx = (opcode & 0x0F00) >> 8;
	uint8_t YY = opcode & 0x00FF;

	registers[Vx] = YY;

}


//ADD Vx, byte: Set Vx = Vx + YY
void Chip8::OP_7XYY()
{

	//Decode X and YY
	uint8_t Vx = (opcode & 0x0F00) >> 8;
	uint8_t YY = opcode & 0x00FF;

	registers[Vx] += YY;

}

//LD Vx, Vy: Set Vx = Vy
void Chip8::OP_8XY0()
{

	//Decode X and Y
	uint8_t Vx = (opcode & 0x0F00) >> 8;
	uint8_t Vy = (opcode & 0x00F0) >> 4;

	registers[Vx] = registers[Vy];

}

//OR Vx, Vy: Set Vx = Vx OR Vy
void Chip8::OP_8XY1()
{

	//Decode X and Y
	uint8_t Vx = (opcode & 0x0F00) >> 8;
	uint8_t Vy = (opcode & 0x00F0) >> 4;

	registers[Vx] |= registers[Vy];

}

//AND Vx, Vy: Set Vx = Vx AND Vy
void Chip8::OP_8XY2()
{

	//Decode X and Y
	uint8_t Vx = (opcode & 0x0F00) >> 8;
	uint8_t Vy = (opcode & 0x00F0) >> 4;

	registers[Vx] &= registers[Vy];

}

//XOR Vx, Vy: Set Vx = Vx XOR Vy
void Chip8::OP_8XY3()
{

	//Decode X and Y
	uint8_t Vx = (opcode & 0x0F00) >> 8;
	uint8_t Vy = (opcode & 0x00F0) >> 4;

	registers[Vx] ^= registers[Vy];

}

//ADD Vx, Vy: Set Vx = Vx + Vy, set VF = carry
void Chip8::OP_8XY4()
{

	//Decode X and Y
	uint8_t Vx = (opcode & 0x0F00) >> 8;
	uint8_t Vy = (opcode & 0x00F0) >> 4;

	//Calculate sum
	uint16_t sum = registers[Vx] + registers[Vy];

	//Set the carry flag
	if (sum > 255)
	{
		registers[0xF] = 1;
	}
	else
	{
		registers[0xF] = 0;
	}

	registers[Vx] = sum & 0x00FF;

}

//SUB Vx, Vy: Set Vx = Vx - Vy, set VF = NOT borrow
void Chip8::OP_8XY5()
{

	//Decode X and Y
	uint8_t Vx = (opcode & 0x0F00) >> 8;
	uint8_t Vy = (opcode & 0x00F0) >> 4;

	//Set the carry flag
	if (registers[Vx] > registers[Vy])
	{
		registers[0xF] = 1;
	}
	else
	{
		registers[0xF] = 0;
	}

	registers[Vx] -= registers[Vy];

}

//SHR Vx: Set Vx = Vx SHR 1
void Chip8::OP_8XY6()
{

	//Decode X
	uint8_t Vx = (opcode & 0x0F00) >> 8;

	//Set the carry flag
	registers[0xF] = registers[Vx] & 0x1;

	//Shift right by 1
	registers[Vx] >>= 1;

}

//SUBN Vx, Vy: Set Vx = Vy - Vx, set VF = NOW borrow
void Chip8::OP_8XY7()
{

	//Decode X and Y
	uint8_t Vx = (opcode & 0x0F00) >> 8;
	uint8_t Vy = (opcode & 0x00F0) >> 4;

	//Set the carry flag
	if (registers[Vy] > registers[Vx])
	{
		registers[0xF] = 1;
	}
	else
	{
		registers[0xF] = 0;
	}

	registers[Vx] = registers[Vy] - registers[Vx];

}

//SHL Vx: Set Vx = Vx SHL 1
void Chip8::OP_8XYE()
{

	//Decode X
	uint8_t Vx = (opcode & 0x0F00) >> 8;

	//Set the carry flag
	registers[0xF] = (registers[Vx] & 0x80) >> 7;

	//Shift left by 1
	registers[Vx] <<= 1;

}

//SNE Vx, Vy: Skip next instruction if Vx != Vy
void Chip8::OP_9XY0()
{

	//Decode X and Y
	uint8_t Vx = (opcode & 0x0F00) >> 8;
	uint8_t Vy = (opcode & 0x00F0) >> 4;

	if (registers[Vx] != registers[Vy])
	{
		programCounter += 2;
	}

}

//LD I, addr: Set I = XXX
void Chip8::OP_AXXX()
{

	//Decode address
	uint16_t addr = opcode & 0x0FFF;

	//Set index register to address
	indexRegister = addr;

}

//JP V0, addr: Jump to location XXX + V0
void Chip8::OP_BXXX()
{

	//Decode XXX
	uint16_t addr = opcode & 0x0FFF;

	//Set programCounter
	programCounter = registers[0] + addr;

}

//RND Vx, byte: Set Vx = random byte AND XX
void Chip8::OP_CXYY()
{

	//Decode X and YY
	uint8_t Vx = (opcode & 0x0F00) >> 8;
	uint8_t YY = opcode & 0x00FF;

	//Generate random number
	registers[Vx] = rand() % 256 && YY; //=========== UPDATE TO BETTER RANDOM NUMBER GENERATION ==========

}

//DRW Vx, Vy, nibble: Display N-byte sprite starting at memory location I at (Vx, Vy), set VF = collision
void Chip8::OP_DXYN()
{

	//Decode X, Y, and N
	uint8_t Vx = (opcode & 0x0F00) >> 8;
	uint8_t Vy = (opcode & 0x00F0) >> 4;
	uint8_t N = opcode & 0x000F;

	//Get xPos and yPos
	uint8_t xPos = registers[Vx] % VIDEO_WIDTH;
	uint8_t yPos = registers[Vy] % VIDEO_HEIGHT;

	registers[0xF] = 0;

	for (uint8_t row = 0; row < N; ++row)
	{

		uint8_t spriteByte = memory[indexRegister + row];

		for (uint8_t col = 0; col < 8; ++col)
		{

			uint8_t spritePixel = spriteByte & (0x80 >> col);
			uint32_t* screenPixel = &display[(yPos + row) * VIDEO_WIDTH + (xPos + col)];

			// Sprite pixel is on
			if (spritePixel)
			{

				// Screen pixel also on - collision
				if (*screenPixel == 0xFFFFFFFF)
				{
					registers[0xF] = 1;
				}

				// Effectively XOR with the sprite pixel
				*screenPixel ^= 0xFFFFFFFF;

			}

		}

	}

}

//SKP Vx: Skip next instruction if key with the value of Vx is pressed
void Chip8::OP_EX9E()
{

	//Decode X
	uint8_t Vx = (opcode & 0x0F00) >> 8;

	//Get key from register
	uint8_t key = registers[Vx];

	if (keypad[key])
	{
		programCounter += 2;
	}

}

//SKNP Vx: Skip next instruction if key with the value of Vx is not pressed
void Chip8::OP_EXA1()
{

	//Decode X
	uint8_t Vx = (opcode & 0x0F00) >> 8;

	//Get key from register
	uint8_t key = registers[Vx];

	if (!keypad[key])
	{
		programCounter += 2;
	}

}

//LD Vx, DT: Set Vx = delay time value
void Chip8::OP_FX07()
{

	//Decode X
	uint8_t Vx = (opcode & 0x0F00) >> 8;

	registers[Vx] = delayTimer;

}

//LD Vx, K: Wait for a key press, store the value of the key in Vx
void Chip8::OP_FX0A()
{

	//Decode X
	uint8_t Vx = (opcode & 0x0F00) >> 8;

	if (keypad[0])
	{
		registers[Vx] = 0;
	}
	else if (keypad[1])
	{
		registers[Vx] = 1;
	}
	else if (keypad[2])
	{
		registers[Vx] = 2;
	}
	else if (keypad[3])
	{
		registers[Vx] = 3;
	}
	else if (keypad[4])
	{
		registers[Vx] = 4;
	}
	else if (keypad[5])
	{
		registers[Vx] = 5;
	}
	else if (keypad[6])
	{
		registers[Vx] = 6;
	}
	else if (keypad[7])
	{
		registers[Vx] = 7;
	}
	else if (keypad[8])
	{
		registers[Vx] = 8;
	}
	else if (keypad[9])
	{
		registers[Vx] = 9;
	}
	else if (keypad[10])
	{
		registers[Vx] = 10;
	}
	else if (keypad[11])
	{
		registers[Vx] = 11;
	}
	else if (keypad[12])
	{
		registers[Vx] = 12;
	}
	else if (keypad[13])
	{
		registers[Vx] = 13;
	}
	else if (keypad[14])
	{
		registers[Vx] = 14;
	}
	else if (keypad[15])
	{
		registers[Vx] = 15;
	}
	else
	{
		programCounter -= 2;
	}

}

//LD DT, Vx: Set delay timer = Vx
void Chip8::OP_FX15()
{

	//Decode X
	uint8_t Vx = (opcode & 0x0F00) >> 8;

	delayTimer = registers[Vx];

}

//LD ST, Vx: Set sound timer = Vx
void Chip8::OP_FX18()
{

	//Decode X
	uint8_t Vx = (opcode & 0x0F00) >> 8;

	soundTimer = registers[Vx];

}

//Add I, Vx: Set I = I + Vx
void Chip8::OP_FX1E()
{

	//Decode X
	uint8_t Vx = (opcode & 0x0F00) >> 8;

	indexRegister += registers[Vx];

}

//LD F, Vx: Set I = Location of sprite for digit Vx
void Chip8::OP_FX29()
{

	//Decode X
	uint8_t Vx = (opcode & 0x0F00) >> 8;
	uint8_t digit = registers[Vx];

	indexRegister = FONTSET_START_ADDRESS + (5 * digit);

}

//LD B, Vx: Store BCD representation of Vx in memory locations I, I+1, and I+2
void Chip8::OP_FX33()
{

	//Decode X
	uint8_t Vx = (opcode & 0x0F00) >> 8;
	uint8_t value = registers[Vx];

	//Ones-place
	memory[indexRegister + 2] = value % 10;
	value /= 10;

	//Tens-place
	memory[indexRegister + 1] = value % 10;
	value /= 10;

	//Hundreds-place
	memory[indexRegister] = value % 10;

}

//LD [I], Vx: Store registers V0 through Vx in memory starting at location I
void Chip8::OP_FX55()
{

	//Decode X
	uint8_t Vx = (opcode & 0x0F00) >> 8;

	//Store registers
	for (uint8_t i = 0; i <= Vx; i++)
	{
		memory[indexRegister + i] = registers[i];
	}

}

//LD Vx, [I]: Read registers V0 through Vx from memory starting at location I
void Chip8::OP_FX65()
{

	//Decode X
	uint8_t Vx = (opcode & 0x0F00) >> 8;

	//Read registers
	for (uint8_t i = 0; i <= Vx; i++)
	{
		registers[i] = memory[indexRegister + i];
	}

}

void Chip8::OP_NULL() { }
