#ifndef SUPER_CHIP8_WINDOW_H
#define SUPER_CHIP8_WINDOW_H

#include "window.h"
#include "SDL.h"
#include "windows.h"
#include "GL/gl.h"

class Superchip8_window : public Window
{
private:
	const static int surface_width = 128;
	const static int surface_height = 64;
	const static int window_width = 640;
	const static int window_height = 320;
	unsigned char* framebuffer;
	GLuint textureId;    
	GLuint pbo;
	uint32_t pbo_size;

public:

	virtual void create_gl_window(int width, int height);
	virtual void scroll_down(int lines);
	virtual void scroll_right(int lines);
	virtual void scroll_left(int lines);
	virtual void draw_image();
	virtual void clear();
	GLuint get_pbo() { return pbo;}
	unsigned char *get_framebuffer() {return framebuffer;}
	Superchip8_window();
	~Superchip8_window();

};

#endif