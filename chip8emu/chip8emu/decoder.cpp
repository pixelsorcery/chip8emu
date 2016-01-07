/*
(*): When drawing the sprites, the sprite to be drawn is pointed to by I. Each byte is one 
row of the sprite. So the number of bytes for the sprite is equal to the number of rows (the 
height of the sprite). However, when drawing a 16x16 sprite, 2 bytes will be used for each 
row since one byte is only 8 bits. If you do not take this into account, you will get many gfx errors.
Also, since the gfx is bit encoded, each pixel is drawn (or erased) only if the bit of that 
pixel is set. Therefore, you should typically do a loop and check each bit if they're set 
[ if(Pixel & (0x80>>line)) ]. The previous sample shows a good way of doing this in a loop, 
where pixel is the byte of the data and line is the current bit to be checked. Also note that 
when drawing a 16x16 sprite, you need to check against 0x8000 since it's 2 bytes.
Last notes. When drawing a pixel, you must first check if it's already printed. If so, then 
that pixel will be erased and the V[F] register will be set. It's quite simple to XOR the pixel 
on the screen to automatically turn it on or off.
Important note: It appears that the system sets V[F] to 0 before drawing any sprites.
Notes: Does the system allow drawing outside the screen, merely ignoring it? Field requires this.

(***): Super chip 8 Instruction. Can be used in CHIP 8 graphic mode.

(****): I is set to point at the hex character of the system's font specified in register X. 
For example, if register X is 1, then I will be set to point to where the data for the 1 font 
is stored in memory. Typically, this will be the beginning of memory (the interpreter memory). 
If it's the FX30 instruction, remember to point I to the super chip 8 font, not the normal chip 
8 font.

(*****): No idea what this really is, but... the following sample code should fix the opcode:
Memory[I]   = (VX) / 100;
Memory[I+1] = (VX /10) % 10;
Memory[I+2] = (VX % 100) % 10;
*/

#include <assert.h>
#include "decoder.h"
#include "chip8_window.h"
#include "superchip8_window.h"

Decoder::Decoder(Context *c, Window *w)
{
	window = w;
	context = c;
}

DECODER_RETURN Decoder::decode(unsigned short opcode)
{
	int x,y;
	char s[256];
	sprintf_s(s, "%x\n", opcode);
	//OutputDebugString(s);
	SDL_Event e;

	//decrement timers
	if(context->sound_timer > 0)
		context->sound_timer--;

	switch((opcode & 0xF000) >> 12)
	{
	case 0x0:

		if(opcode == 0xC0)
		{
			//scroll down 0X000N lines
			window->scroll_down(opcode & 0x000F);
			return INCREMENT;
		}

		switch(opcode & 0x00FF)
		{
		case 0xFB:
			//scroll right instead of down and ALWAYS 4 pixels (2 if used in normal chip 8 mode).
			window->scroll_right();
			break;
		case 0xFC:
			//Same as above; but left 
			window->scroll_left();
			break;
		case 0xFD:
			//Quit the emulator 
			return QUIT;
			break;
		case 0xFE:
			//Set chip 8 graphic mode (64x32) 
			break;
		case 0xFF:
			//Set super chip 8 graphic mode (128x64) 
			//TODO: make this return code
			assert(!"implement this shit motherfucker!");
			break;
		case 0xE0:
			//clear the screen
			window->clear();
			break;
		case 0xEE:
			//Return from a CHIP-8 sub-routine.
			assert(context->stack_pos > 0);
			context->stack_pos--;
			context->instruction = context->stack[context->stack_pos];
			break;
		}

		break;
	case 0x1:
		//jump to 0x0nnn
		context->instruction = opcode & 0x0FFF;
		return NO_INCREMENT;
		break;
	case 0x2:
		//call subroutine at 0x0nnn
		context->stack[context->stack_pos] = context->instruction;
		context->stack_pos++;
		context->instruction = opcode & 0x0FFF;
		break;
	case 0x3:
		//3XKK		Skip next instruction if VX == KK
		if (context->registerfile[(opcode & 0x0F00) >> 8] == (opcode & 0x00FF))
			//TODO: return skip next instruction code
			return SKIP;
		break;
	case 0x4:
		//4XKK		Skip next instruction if VX != KK
		if(context->registerfile[(opcode & 0x0F00) >> 8] != (opcode & 0x00FF))
			return SKIP;
		break;
	case 0x5:
		//5XY0		Skip next instruction if VX == VY
		if(context->registerfile[(opcode & 0x0F00) >> 8] == context->registerfile[(opcode & 0x00F0) >> 4] )
			//TODO: return skip next instruction code
			return SKIP;
		break;
	case 0x6:
		//6XKK		VX = KK (move the value of KK into register specified in X)
		context->registerfile[(opcode & 0x0F00) >> 8] = opcode & 0x00FF;
		break;
	case 0x7:
		//7XKK		VX = VX + KK (add the value of KK into register X)
		context->registerfile[(opcode & 0x0F00) >> 8] += opcode & 0x00FF;
		break;
	case 0x8:
		switch(opcode & 0x800F)
		{
		case 0x8000:
			//8XY0		VX = VY (move register Y into register X)
			context->registerfile[(opcode & 0x0F00) >> 8] = context->registerfile[(opcode & 0x00F0) >> 4]; 
			break;
		case 0x8001:
			//8XY1		VX = VX OR VY (Binary OR register X with register Y)
			context->registerfile[(opcode & 0x0F00) >> 8] = context->registerfile[(opcode & 0x0F00) >> 8] | context->registerfile[(opcode & 0x00F0) >> 4]; 
			break;
		case 0x8002:
			//8XY2		VX = VX AND VY (Binary AND register X with register Y)
			context->registerfile[(opcode & 0x0F00) >> 8] = context->registerfile[(opcode & 0x0F00) >> 8] & context->registerfile[(opcode & 0x00F0) >> 4]; 
			break;
		case 0x8003:
			//8XY3		VX = VX XOR VY (Binary XOR register X with register Y)
			context->registerfile[(opcode & 0x0F00) >> 8] = context->registerfile[(opcode & 0x0F00) >> 8] ^ context->registerfile[(opcode & 0x00F0) >> 4]; 
			break;
		case 0x8004:
			//8XY4	VX = VX + VY, VF = carry (Add register Y into register X. Set 
			//VF if carry [That means that if the X + Y > 255 there is carry, 
			//since each register is only 1 byte, hence if the result would be 
			//greater the value would overlap and begin from 0 again.].)
			x = context->registerfile[(opcode & 0x0F00) >> 8];
			y = context->registerfile[(opcode & 0x00F0) >> 4];
			if (x + y > 255) 
				context->registerfile[0xF] = 1;
			else
				context->registerfile[0XF] = 0;
			context->registerfile[(opcode & 0x0F00) >> 8] += context->registerfile[(opcode & 0x00F0) >> 4];
			break;
		case 0x8005:
			//8XY5	VX = VX - VY, VF = not borrow (**) (Subtract register X with 
			//register Y. If the result is lesser than 0, then borrow should NOT 
			//be set; otherwise it should, because the carry register is set to NOT borrow) 
			//Note: Seems to break collision detection in pong. To fix it, set carry register if borrow.
			x = context->registerfile[(opcode & 0x0F00) >> 8];
			y = context->registerfile[(opcode & 0x00F0) >> 4];
			if (x < y) 
				context->registerfile[0xF] = 0;
			else
				context->registerfile[0xF] = 1;
			context->registerfile[(opcode & 0x0F00) >> 8] -= context->registerfile[(opcode & 0x00F0) >> 4];
			break;
		case 0x8006:
			//8XY6	VX = VX >> 1 VF = carry (Shift right 1 bit. Bit 1 bit must be saved in carry.) (Carry is unconfirmed)
			context->registerfile[0xF] = 0x1 & context->registerfile[(opcode & 0x0F00) >> 8];
			context->registerfile[(opcode & 0x0F00) >> 8] = context->registerfile[(opcode & 0x0F00) >> 8] >> 1;
			break;
		case 0x8007:
			//8XY7	VX = VY - VX, VF = not borrow (**) (if register Y is greater than or equal to (>=) register X, set borrow)
			x = context->registerfile[(opcode & 0x0F00) >> 8];
			y = context->registerfile[(opcode & 0x00F0) >> 4];
			if (x < y) 
				context->registerfile[0xF] = 1;
			else
				context->registerfile[0xF] = 0;
			break;
		case 0x800E:
			//8XYE	VX = VX << 1 (VX=VX*2), VF = carry (Shift left 1 bit. Bit 1 bit must be saved in carry.) (Carry is unconfirmed)
			context->registerfile[0xF] = 0x80 & context->registerfile[(opcode & 0x0F00) >> 8];
			context->registerfile[(opcode & 0x0F00) >> 8] = context->registerfile[(opcode & 0x0F00) >> 8] << 1;
			break;
		}

		break;
	case 0x9:
		//9XY0		Skip next instruction if VX != VY
		if(context->registerfile[(opcode & 0x0F00) >> 8] != context->registerfile[(opcode & 0x00F0) >> 4] )
			return SKIP;
		break;
	case 0xA:
		//ANNN	I = NNN (Move NNN into I register)
		context->address = opcode & 0xFFF;
		break;
	case 0xB:
		//BNNN	Jump to NNN + V0 (Same as 1NNN instruction. Only, it also adds the sum of register 0 to the jump address.)
		context->instruction = (opcode & 0x0FFF) + context->registerfile[0];
		break;
	case 0xC:
		//CXKK	VX = Random number AND KK (Use a random number and use binary AND on it. 
		//Then store it in register X. Actually, I am unsure what method you should use, 
		//as many does this differently. However, I use binary AND to achieve results and it works fine for me.)
		context->registerfile[(opcode & 0x0F00) >> 8] = (rand() % 255) & (opcode & 0x00FF);
		break;
	case 0xD:
		//DXYN	Draws an 8xN (width x height) sprite at the coordinates given in register X and Y. 
		//If height specified is 0, draws a 16x16 sprite. See notes below. (*)
		x = (opcode & 0x0F00) >> 8;
		y = (opcode & 0x00F0) >> 4;

		if ((opcode & 0x000F) == 0)
			window->draw_sprite(x, y, 16, 16, context->mem + context->address); //todo fix this
		else
			if (window->draw_sprite(context->registerfile[x], context->registerfile[y], 8, (opcode & 0x000F), context->mem + context->address))
				context->registerfile[0XF] = 1;
			else
				context->registerfile[0XF] = 0;
		break;
	case 0xE:
		switch(opcode & 0xF0FF)
		{
		case 0xE09E:
			//EX9E		Skip next instruction if key in register X is pressed
			if (context->keys[context->registerfile[(opcode & 0x0F00) >> 8]])
				return SKIP;
			break;
		case 0xE0A1:
			//EXA1		Skip next instruction if key in register X is not pressed
			if (!context->keys[context->registerfile[(opcode & 0x0F00) >> 8]])
				return SKIP;
			break;
		}
		break;
	case 0xF:
		switch(opcode & 0xF0FF)
		{
		case 0xF007:
			//VX = Delay timer (put the value of the delay timer into register X)
			context->registerfile[(opcode & 0x0F00) >> 8] = context->delay_timer;
			break;
		case 0xF00A:
			//Waits a key press and stores it in VX (basically halts the emulation until a key press is recorded.)
			while (!SDL_PollEvent (&e) || e.type!=2) ;
			break;
		case 0xF015:
			//FX15		Delay timer = VX
			context->delay_timer = opcode && 0x00FF;
			break;
		case 0xF018:
			//FX18		Sound timer = VX
			context->sound_timer = opcode && 0x00FF;
			break;
		case 0xF01E:
			//FX1E		I = I + VX
			context->address += context->registerfile[(opcode & 0x0F00) >> 8];
			break;
		case 0xF029:
			//FX29		I points to the 4 x 5 font sprite of hex char in VX (****)
			context->address = context->registerfile[(opcode & 0x0F00) >> 8] * 5;
			break;
		case 0xF030:
			//FX30		I points to the 8 x 10 font sprite of hex char in VX (****)
			assert(!"Super chip 8 not implemented TODO motherfucker!");
			break;
		case 0xF033:
			//FX33		Store BCD representation of VX in I...I+2 (*****)
			context->mem[context->address] = context->registerfile[(opcode & 0x0F00) >> 8] / 100;
			context->mem[context->address+1] = (context->registerfile[(opcode & 0x0F00) >> 8] / 10) % 10;
			context->mem[context->address+2] = context->registerfile[(opcode & 0x0F00) >> 8] % 10;
			break;
		case 0xF055:
			//FX55	Save V0...VX in memory starting at I. Does NOT increment I register.
			memcpy(context->mem + context->address, context->registerfile, ((opcode & 0x0F00) >> 8) + 1);
			break;
		case 0xF065:
			//FX65	Load V0...VX from memory starting at I. Does NOT increment I register.
			memcpy(context->registerfile, context->mem + context->address, ((opcode & 0x0F00) >> 8) + 1);
			break;
		case 0xF075:
			//FX75	Save V0...VX (X<8) in the HP48 flags (Simple as just to save it in 
			//a variable) Does NOT increment I register. (***)
			//NOTE: Are these flags pre-initialized with some values?
			assert(!"IMPLEMENT THIS!");
			break;
		case 0xF085:
			//FX85	Load V0...VX (X<8) from the HP48 flags (Same as above, but load) 
			//Does NOT increment I register. (***)
			assert(!"IMPLEMENT THIS!");
			break;
		default:
			assert(false);
			break;
		}
		break;
	default:
		printf("%x: unknown instruction.\n", opcode);
		return ERR;
		break;
	}
	return INCREMENT;
}
