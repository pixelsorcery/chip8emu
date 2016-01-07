#ifndef WINDOW_H
#define WINDOW_H

class Window
{
public:
	virtual void create_gl_window(int width, int height) = 0;
	virtual void scroll_down(int lines) = 0;
	virtual void scroll_right(int lines = 2) = 0;
	virtual void scroll_left(int lines = 2) = 0;
	virtual void draw_image() = 0;
	virtual void clear() = 0;
	virtual int draw_sprite(int x, int y, int width, int height, unsigned char *) = 0;
	virtual ~Window(){};
};

#endif