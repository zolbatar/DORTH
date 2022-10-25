#ifndef FB_H
#define FB_H
#include "../beebScreen/beebScreen.h"
#define STBTT_STATIC
#define STB_TRUETYPE_IMPLEMENTATION
#include "fonts/stb_truetype.h"
#include <stdbool.h>

#define WIDTH 640
#define HEIGHT 256
#define SIZE WIDTH *HEIGHT

extern int minX;
extern int maxX;
extern int minY;
extern int maxY;

void fb_init();
void fb_plot(int x, int y);
void fb_plot_black(int x, int y);
void fb_line(int x0, int y0, int x1, int y1);
void fb_rect(int x0, int y0, int x1, int y1);
void fb_rect_filled(int x0, int y0, int x1, int y1, bool filled);
void fb_triangle(int x1, int y1, int x2, int y2, int x3, int y3, bool filled);
void fb_circle(int xc, int yc, int r, bool fill);
void fb_char(char c, uint32_t x, uint32_t y);
void fb_string(const char* str, uint32_t x, uint32_t y);
void fb_char_small(char c, uint32_t x, uint32_t y);
void fb_string_small(const char* str, uint32_t x, uint32_t y);
void fb_char_square(char c, uint32_t x, uint32_t y);
void fb_string_square(const char* str, uint32_t x, uint32_t y);
void ttf_init();
void ttf_set_threshold(int v);
void ttf_print_text(const stbtt_fontinfo* ff, float size, const char* text, int cursor_x, int cursor_y);

extern unsigned char* buffer;

#endif // FB_H
