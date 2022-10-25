#include <stdlib.h>
#include "../fb.h"
#include "font.h"
#ifdef SDL
#ifdef WINDOWS
#define GL_GLEXT_PROTOTYPES 1
#include <SDL.h>
#include <SDL_opengl.h>
#else
#define GL_GLEXT_PROTOTYPES 1
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#endif
extern float dpi_ratio;
extern SDL_Renderer* renderer;
#endif

void fb_char_small(char c, uint32_t x, uint32_t y)
{
	// Convert the character to an index
	c = c & 0x7F;
	if (c<' ') {
		c = 0;
	}
	else {
		c -= ' ';
	}

	// 'font' is a multidimensional array of [96][char_width]
	// which is really just a 1D array of size 96*char_width.
	const unsigned char* chr = &fontpzim3x5[c*CHAR_WIDTH_SMALL];

	// Draw pixels
	for (uint8_t j = 0; j<CHAR_WIDTH_SMALL; j++) {
		const unsigned char cc = chr[j];
		//		printf("%d:", cc);
		for (uint8_t i = 0; i<CHAR_HEIGHT; i++) {

			int bm = 1 << i;
			if (cc & bm) {
				//				printf("#");
				fb_plot(x+j, y+i);
			}
			else {
				//				printf(" ");
			}
		}
		//		printf("\n");
	}
}

void fb_string_small(const char* str, uint32_t x, uint32_t y)
{
	while (*str) {
		fb_char_small(*str++, x, y);
		x += (CHAR_WIDTH_SMALL+1);
	}
}

void fb_char(char c, uint32_t x, uint32_t y)
{
	// Convert the character to an index
	c = c & 0x7F;
	if (c<' ') {
		c = 0;
	}
	else {
		c -= ' ';
	}

	// 'font' is a multidimensional array of [96][char_width]
	// which is really just a 1D array of size 96*char_width.
	const unsigned char* chr = &font[c*CHAR_WIDTH];

	// Draw pixels
	for (uint8_t j = 0; j<CHAR_WIDTH; j++) {
		const unsigned char cc = chr[j];
		//		printf("%d:", cc);
		for (uint8_t i = 0; i<CHAR_HEIGHT; i++) {

			int bm = 1 << i;
			if (cc & bm) {
				//				printf("#");
				fb_plot(x+j, y+i);
			}
			else {
				//				printf(" ");
			}
		}
		//		printf("\n");
	}
}

void fb_string(const char* str, uint32_t x, uint32_t y)
{
	while (*str) {
		fb_char(*str++, x, y);
		x += (CHAR_WIDTH);
	}
}

void fb_char_square(char c, uint32_t x, uint32_t y)
{
	// Convert the character to an index
	c = c & 0x7F;
	if (c<' ') {
		c = 0;
	}
	else {
		c -= ' ';
	}

	// 'font' is a multidimensional array of [96][char_width]
	// which is really just a 1D array of size 96*char_width.
	const unsigned char* chr = &font8x8_basic[c*CHAR_WIDTH_SQUARE];

	// Draw pixels
	for (uint8_t j = 0; j<CHAR_WIDTH_SQUARE; j++) {
		const unsigned char cc = chr[j];
		//		printf("%d:", cc);
		for (uint8_t i = 0; i<CHAR_HEIGHT; i++) {

			int bm = 1 << i;
			if (cc & bm) {
				//				printf("#");
				fb_plot(x+i, y+j);
			}
			else {
				//				printf(" ");
			}
		}
		//		printf("\n");
	}
}

void fb_string_square(const char* str, uint32_t x, uint32_t y)
{
	while (*str) {
		fb_char_square(*str++, x, y);
		x += (CHAR_WIDTH_SQUARE);
	}
}