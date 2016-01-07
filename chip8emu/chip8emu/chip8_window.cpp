
#include "windows.h"
#include "GL/glew.h"
#include "GL/gl.h"
#include "chip8_window.h"

Chip8_window::Chip8_window()
{
	SDL_Init(32);
	SDL_SetVideoMode(window_width, window_height, 8, 2);
	pbo_size = surface_width * surface_height;
	framebuffer = new unsigned char[pbo_size];
	for(unsigned int i = 0; i < pbo_size; ++i)
		framebuffer[i] = 0;

	GLenum err = glewInit();
	if(err != GLEW_OK)
	{
		return;
	}

	//gl settings
	glShadeModel(GL_FLAT);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glDisable(GL_CULL_FACE);
    glEnable(GL_TEXTURE_2D);

	//generate texture
	glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexImage2D(GL_TEXTURE_2D, 0, 1, surface_width, surface_height, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, NULL);

	// create buffer object
    glGenBuffers(1, &pbo);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pbo);
}

int Chip8_window::draw_sprite(int x, int y, int width, int height, unsigned char *sprite)
{
    glBindTexture(GL_TEXTURE_2D, textureId);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pbo);

	glBufferData(GL_PIXEL_UNPACK_BUFFER, pbo_size, NULL, GL_DYNAMIC_DRAW);

	//get pointer to pbo
	unsigned char *pbuf = (unsigned char *)glMapBuffer(GL_PIXEL_UNPACK_BUFFER, GL_WRITE_ONLY);
	pixels_drawn = false;

	char index = 0;
	char bit = 7;
	int return_val = 0; 
	for (int j = y; j < height + y; j++)
	{
		for (int i = x; i < width + x; i++)
		{
			if (((sprite[index] >> bit) & 0x1) && pbuf[(j * surface_width) + i])
			{
				pbuf[(j * surface_width) + i] = 0;
				return_val = 1;
			}
			else if (((sprite[index] >> bit) & 0x1) && !pbuf[(j * surface_width) + i])
			{
				pbuf[(j * surface_width) + i] = 255;
				pixels_drawn = true;
			}
			bit--;
			if(bit == -1)
			{
				index++;
				bit = 7;
			}
		}
	}
	//release
	glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER); 
	//write to texture - fast, returns immediately
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, surface_width, surface_height, GL_LUMINANCE, GL_UNSIGNED_BYTE, NULL);

	return return_val;
}

void Chip8_window::draw_image()
{
	if (!pixels_drawn)
		return;
	//draw everything
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 

    glBegin(GL_QUADS); 
        glTexCoord2f(0.0, 1.0); 
        glVertex3f(-1.0, -1.0, 0.0); 

        glTexCoord2f(0.0, 0.0); 
        glVertex3f(-1.0, 1.0, 0.0); 

        glTexCoord2f(1.0, 0.0); 
        glVertex3f(1.0, 1.0, 0.0); 

        glTexCoord2f(1.0, 1.0); 
        glVertex3f(1.0, -1.0, 0.0); 
    glEnd(); 
	SDL_GL_SwapBuffers();
}

void Chip8_window::create_gl_window(int width, int height)
{

}

void Chip8_window::scroll_down(int lines)
{
	if(lines == 0) return;
	//get source and dest pointers and amount of 
	//lines to move which basically copies the whole screen
	//TODO: should the rest of the screen be cleared? - yes for now
	unsigned char *src = framebuffer;
	unsigned char *dst = framebuffer + (lines * surface_width);
	size_t move_size = (surface_width * surface_height) - (lines * surface_width);
	memmove(dst, src, move_size);

	//clear the lines we moved from
	memset(framebuffer, 23, lines*surface_width);
}

void Chip8_window::scroll_right(int lines)
{
	if(lines == 0) return;

	for(int y = 0; y < surface_height; ++y)
	{
		for(int x = surface_width - 1; x > lines-1; --x)
		{
			//move each pixel x pixels to the right
			int src_index = y * surface_width + x - lines;
			int dst_index = y * surface_width + x;
			framebuffer[dst_index] = framebuffer[src_index];
			//clear the pixel after we move it
			framebuffer[src_index] = 255;
		}
	}		
}

void Chip8_window::scroll_left(int lines)
{
	if(lines == 0) return;

	for(int y = 0; y < surface_height; ++y)
	{
		for(int x = lines; x < surface_width; ++x)
		{
			//move each pixel x pixels to the left
			int src_index = y * surface_width + x;
			int dst_index = y * surface_width + x - lines;
			framebuffer[dst_index] = framebuffer[src_index];
			//clear out the pixel after we move it
			framebuffer[src_index] = 255; 
		}
	}	
}

void Chip8_window::clear()
{
	memset(framebuffer, 0, surface_height*surface_width);
}

Chip8_window::~Chip8_window()
{
	delete framebuffer;
	glDeleteBuffers(1, &pbo);
	glDeleteTextures(1, &textureId);
}

