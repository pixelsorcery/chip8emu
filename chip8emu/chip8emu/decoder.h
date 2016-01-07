#ifndef DECODER_H
#define DECODER_H

#include "context.h"
#include "window.h"

typedef enum{
	INCREMENT = 0,
	NO_INCREMENT = 1,
	ERR = 2,
	QUIT = 3,
	SKIP = 4
} DECODER_RETURN;

class Decoder
{
private:
	Context *context;
	Window *window;
public:
	Decoder(Context *, Window *); //context and window
	DECODER_RETURN decode(unsigned short opcode);
};
#endif