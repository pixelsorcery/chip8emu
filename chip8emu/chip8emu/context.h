#ifndef CONTEXT_H
#define CONTEXT_H

#include "memory.h"

class Context
{
public:
	Context();
	~Context();

	unsigned char *mem;
	char *registerfile;
	unsigned short *stack;
	bool *keys;
	unsigned short codereg;
	unsigned short address;
	unsigned char delay_timer;
	unsigned char sound_timer;
	int increment_instruction();
	int instruction;
	int stack_pos;
};

#endif
