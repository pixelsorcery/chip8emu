#ifndef CHIP8_WINDOW_H
#define CHIP8_WINDOW_H

#include "window.h"
#include "SDL.h"
#include "windows.h"
#include "GL/gl.h"

class Chip8_window : public Window
{
private:
	const static int surface_width = 64;
	const static int surface_height = 32;
	const static int window_width = 640;
	const static int window_height = 320;
	unsigned char* framebuffer;
	GLuint textureId;    
	GLuint pbo;
	uint32_t pbo_size;
	bool pixels_drawn;

public:

	virtual void create_gl_window(int width, int height);
	virtual void scroll_down(int lines);
	virtual void scroll_right(int lines = 2);
	virtual void scroll_left(int lines = 2);
	virtual void draw_image();
	GLuint get_pbo() { return pbo;}
	unsigned char *get_framebuffer() {return framebuffer;}
	virtual void clear();
	virtual int draw_sprite(int x, int y, int width, int height, unsigned char *);
	Chip8_window();
	~Chip8_window();

};

#endif