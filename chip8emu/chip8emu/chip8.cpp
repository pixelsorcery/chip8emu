#include "chip8.h"
#include "chip8_window.h"

Chip8::Chip8()
{
	chip8window = new Chip8_window();
	chip8context = new Context();
}

int Chip8::load_game(char *filename)
{
	FILE *pFile;
	unsigned int size;
	
	errno_t err = fopen_s(&pFile, filename, "rb");
	fseek(pFile, 0, SEEK_END);
	size = ftell(pFile);
	rewind(pFile);

	int result = fread((chip8context->mem + 512), 1, size, pFile);

	fclose(pFile);

	chip8context->codereg = chip8context->mem[chip8context->instruction++] << 8;
	chip8context->codereg |= chip8context->mem[chip8context->instruction++];

	return result;
}
