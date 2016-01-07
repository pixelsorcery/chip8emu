#ifndef CHIP8_H
#define CHIP8_H

#include "window.h"
#include "chip8_window.h"
#include "context.h"
#include "SDL.h"

class Chip8
{
public:
	Chip8_window *chip8window;
	Context *chip8context;
	Chip8();
	int load_game(char *filename);
	~Chip8();
};
#endif