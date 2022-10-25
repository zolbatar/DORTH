#include <inttypes.h>
#include <string.h>
#include "../fb.h"
#ifdef SDL
#ifdef WINDOWS
#define GL_GLEXT_PROTOTYPES 1
#include <SDL.h>
#else
#define GL_GLEXT_PROTOTYPES 1
#include <SDL2/SDL.h>
#endif
extern float dpi_ratio;
extern SDL_Renderer *renderer;
#endif

int last_cursor_x = 0;
int last_cursor_y = 0;
int threshold = 8;

typedef struct
{
    int width;
    int height;
    float scale;
    int baseline;
    unsigned char *bitmap;
    int ix0, iy0, ix1, iy1;
    int advance, lsb;
    int sc_width;
} glyph_t;

void ttf_init()
{
}

glyph_t ttf_get_glyph_x(const stbtt_fontinfo *ff, float size, uint8_t ascii)
{
    // Create the glyph
    glyph_t f;
    f.bitmap = NULL;
    f.scale = stbtt_ScaleForPixelHeight(ff, size);
    f.bitmap = stbtt_GetCodepointBitmap(ff, 0, f.scale, ascii, &f.width, &f.height, 0, 0);

    // Baseline
    int ascent;
    stbtt_GetFontVMetrics(ff, &ascent, 0, 0);
    f.baseline = (int)(ascent * f.scale);

    // Font adjust
    stbtt_GetCodepointHMetrics(ff, ascii, &f.advance, &f.lsb);
    stbtt_GetCodepointBitmapBox(ff, ascii, f.scale, f.scale, &f.ix0, &f.iy0, &f.ix1, &f.iy1);
    f.sc_width = (int)(f.advance * f.scale);

    return f;
}

int ttf_max_horz_chars(int typeface, int size)
{
    glyph_t f = ttf_get_glyph_x(&fontMono, size, ' ');
    return WIDTH / f.sc_width;
}

int ttf_get_font_height(const stbtt_fontinfo *ff, float size)
{
    // Row height
    glyph_t f = ttf_get_glyph_x(ff, size, ' ');

    // Calc
    int m = 0;
    for (int c = 32; c <= 127; c++)
    {
        int ix0, iy0, ix1, iy1;
        stbtt_GetCodepointBitmapBox(ff, c, f.scale, f.scale, &ix0, &iy0, &ix1, &iy1);
        if (iy1 > m)
        {
            m = iy1;
        }
    }
    return f.baseline + m;
}

void ttf_print_character(const stbtt_fontinfo *ff, float size, char c, int *cursor_x, int *cursor_y)
{
    int font_row_height = ttf_get_font_height(ff, size);

    switch (c)
    {
    case '\r':
        *cursor_y += font_row_height;
        *cursor_x = 0;
        return;
    case '\n':
        *cursor_y += font_row_height;
        *cursor_x = 0;
        return;
    }
    glyph_t f = ttf_get_glyph_x(ff, size, c);
    if (f.bitmap != NULL)
    {
        int xs = *cursor_x + f.ix0;
        int ys = *cursor_y + f.iy0 + f.baseline;
        int idx = 0;
        for (int j = 0; j < f.height; ++j)
        {
            for (int i = 0; i < f.width; ++i)
            {
                int v = f.bitmap[idx++];
                if (v >= threshold)
                {
                    int x = xs + i;
                    int y = ys + j;
                    fb_plot(x, y);
                    //					g_env.graphics.current_colour.set_a(v);
                }
            }
        }
    }
    *cursor_x += f.sc_width;

    // End of line?
    while (*cursor_x + f.sc_width >= WIDTH)
    {
        *cursor_y += font_row_height;
        *cursor_x = 0;
    }
}

void ttf_set_threshold(int v)
{
    threshold = v;
}

void ttf_print_text(const stbtt_fontinfo *ff, float size, const char *text, int cursor_x, int cursor_y)
{
    if (cursor_x == -1)
        cursor_x = last_cursor_x;
    if (cursor_y == -1)
        cursor_y = last_cursor_y;
    while (*text)
    {
        ttf_print_character(ff, size, *text++, &cursor_x, &cursor_y);
    }
    last_cursor_x = cursor_x;
    last_cursor_y = cursor_y;
}