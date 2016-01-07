#include <iostream>
#include <fstream>
#include <string>
#include <assert.h>
#include "windows.h"
#include "GL/glew.h"
#include "GL/gl.h"
#include "SDL.h"
#include "chip8_window.h"
#include "chip8.h"
#include "superchip8_window.h"
#include "decoder.h"

using namespace std;

int main (int c, char **a)
{
	if(c != 2)
	{
		cout << "Usage: chip8 [inputfile]" <<endl;
		return -1;
	}

	//Create chip8
	Chip8 *chip8dev = new Chip8();

	//read game into memory
	chip8dev->load_game(a[1]);
	Decoder *myDecoder = new Decoder(chip8dev->chip8context, chip8dev->chip8window);
	Chip8_window *myWindow = chip8dev->chip8window;
	//myWindow->scroll_down(2);
	//myWindow->scroll_right(2);
	//myWindow->scroll_left(1);

	SDL_Event e;

	bool exit = false;
	unsigned int last_draw_time, current_time, last_timer_decrement;

	current_time = SDL_GetTicks();
	last_draw_time = current_time;
	last_timer_decrement = current_time;

	while (!exit)
    {
		current_time = SDL_GetTicks();
		DECODER_RETURN decoder_result;
		//decode/execute it
		decoder_result = myDecoder->decode(chip8dev->chip8context->codereg);

		switch(decoder_result)
		{
			case INCREMENT:
			case NO_INCREMENT:
				chip8dev->chip8context->increment_instruction();
				break;
			case SKIP:
				chip8dev->chip8context->increment_instruction();
				chip8dev->chip8context->increment_instruction();
				break;
			case ERR:
				assert(false);
			case QUIT:
				exit = true;
				break;
		}

		if (SDL_PollEvent(&e))
		{
			if (e.type == SDL_QUIT) //press x in corner of window
				exit = true;
			else if (e.type == SDL_KEYDOWN)
			{
				SDLKey keyPressed = e.key.keysym.sym;
				switch (keyPressed)
				{
				case SDLK_1:
					chip8dev->chip8context->keys[1] = true;
					break;
				case SDLK_2:
					chip8dev->chip8context->keys[2] = true;
					break;
				case SDLK_3:
					chip8dev->chip8context->keys[3] = true;
					break;
				case SDLK_4:
					chip8dev->chip8context->keys[0xC] = true;
					break;
				case SDLK_q:
					chip8dev->chip8context->keys[4] = true;
					break;
				case SDLK_w:
					chip8dev->chip8context->keys[5] = true;
					break;
				case SDLK_e:
					chip8dev->chip8context->keys[6] = true;
					break;
				case SDLK_r:
					chip8dev->chip8context->keys[0xD] = true;
					break;
				case SDLK_a:
					chip8dev->chip8context->keys[7] = true;
					break;
				case SDLK_s:
					chip8dev->chip8context->keys[8] = true;
					break;
				case SDLK_d:
					chip8dev->chip8context->keys[9] = true;
					break;
				case SDLK_f:
					chip8dev->chip8context->keys[0xE] = true;
					break;
				case SDLK_z:
					chip8dev->chip8context->keys[0xA] = true;
					break;
				case SDLK_x:
					chip8dev->chip8context->keys[0] = true;
					break;
				case SDLK_c:
					chip8dev->chip8context->keys[0xB] = true;
					break;
				case SDLK_v:
					chip8dev->chip8context->keys[0xF] = true;
					break;		
				case SDLK_ESCAPE:
					exit = true;
					break;
				}
			}
			else if (e.type == SDL_KEYUP)
			{
				SDLKey keyPressed = e.key.keysym.sym;
				switch (keyPressed)
				{
				case SDLK_1:
					chip8dev->chip8context->keys[1] = false;
					break;
				case SDLK_2:
					chip8dev->chip8context->keys[2] = false;
					break;
				case SDLK_3:
					chip8dev->chip8context->keys[3] = false;
					break;
				case SDLK_4:
					chip8dev->chip8context->keys[0xC] = false;
					break;
				case SDLK_q:
					chip8dev->chip8context->keys[4] = false;
					break;
				case SDLK_w:
					chip8dev->chip8context->keys[5] = false;
					break;
				case SDLK_e:
					chip8dev->chip8context->keys[6] = false;
					break;
				case SDLK_r:
					chip8dev->chip8context->keys[0xD] = false;
					break;
				case SDLK_a:
					chip8dev->chip8context->keys[7] = false;
					break;
				case SDLK_s:
					chip8dev->chip8context->keys[8] = false;
					break;
				case SDLK_d:
					chip8dev->chip8context->keys[9] = false;
					break;
				case SDLK_f:
					chip8dev->chip8context->keys[0xE] = false;
					break;
				case SDLK_z:
					chip8dev->chip8context->keys[0xA] = false;
					break;
				case SDLK_x:
					chip8dev->chip8context->keys[0] = false;
					break;
				case SDLK_c:
					chip8dev->chip8context->keys[0xB] = false;
					break;
				case SDLK_v:
					chip8dev->chip8context->keys[0xF] = false;
					break;	
				}
			}
		}

		chip8dev->chip8window->draw_image();
		last_draw_time = current_time;

		if(chip8dev->chip8context->delay_timer && 
			current_time - last_timer_decrement > 1000)
		{
			chip8dev->chip8context->delay_timer--;
			last_timer_decrement = current_time;
		}

		if(chip8dev->chip8context->sound_timer && 
			current_time - last_timer_decrement > 1000)
		{
			chip8dev->chip8context->sound_timer -= current_time - last_timer_decrement % 1000;
			last_timer_decrement = current_time;
		}
  
	}
	return 0;
}

