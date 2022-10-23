#include <stdbool.h>
#include <inttypes.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "fb.h"
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

#define max(x, y) (((x) > (y)) ? (x) : (y))
#define min(x, y) (((x) < (y)) ? (x) : (y))

extern bool usePi;
unsigned char *buffer; //[SIZE];
int pal[] = {0x0000, 0x0018, 0x8020, 0x8038, 0x0840, 0x0858, 0x8068,
             0x8878, 0x4484, 0x009f, 0xf0a0, 0xf0bf, 0x0fcf, 0xffff};
int palMono[] = {0x0000, 0xFFFF};
int palGreyScale[16] = {0x0000, 0x1101, 0x2202, 0x3303, 0x4404, 0x5505, 0x6606, 0x7707,
                        0x8808, 0x9909, 0xaa0a, 0xbb0b, 0xcc0c, 0xdd0d, 0xee0e, 0xff0f};

int minX = 0;
int maxX = WIDTH - 1;
int minY = 0;
int maxY = HEIGHT - 1;

void fb_init()
{
    buffer = malloc(SIZE);
    if (usePi)
    {
        beebScreen_Init(0, BS_INIT_DOUBLE_BUFFER | BS_INIT_PIVDU);
    }
    else
    {
        beebScreen_Init(0, BS_INIT_NORMAL);
    }
    beebScreen_SetBuffer(buffer, BS_BUFFER_FORMAT_8BPP, WIDTH, HEIGHT);
    beebScreen_SetGeometry(WIDTH, HEIGHT, TRUE);
    if (!usePi)
    {
        beebScreen_UseDefaultScreenBases();

        // Clear any unused screen memory (this is slow :'( )
        //		beebScreen_ClearScreens(true);
    }
    beebScreen_SendPal(palMono, 2);
    // beebScreen_SendPal(palGreyScale, 16);

    // Clear buffer
    memset(buffer, 0, SIZE);

//	ttf_init();
#ifdef SDL
    SDL_RenderClear(renderer);
#endif
}

typedef int OutCode;

const int INSIDE = 0; // 0000
const int LEFT = 1;   // 0001
const int RIGHT = 2;  // 0010
const int BOTTOM = 4; // 0100
const int TOP = 8;    // 1000

void fb_plot(int x, int y)
{
    if (x < minX || x > maxX || y < minY || y > maxY)
        return;
    size_t index = (y * WIDTH) + x;
#ifdef SDL
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_Rect r;
    r.x = x;
    r.y = y;
    r.w = 1;
    r.h = 1;
    SDL_RenderFillRect(renderer, &r);
#else
    buffer[index] = 1;
#endif
}

void fb_plot_black(int x, int y)
{
    if (x < minX || x > maxX || y < minY || y > maxY)
        return;
    size_t index = (y * WIDTH) + x;
#ifdef SDL
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_Rect r;
    r.x = x;
    r.y = y;
    r.w = 1;
    r.h = 1;
    SDL_RenderFillRect(renderer, &r);
#else
    buffer[index] = 0;
#endif
}

void fb_rect(int x0, int y0, int x1, int y1)
{
    fb_line(x0, y0, x1, y0);
    fb_line(x0, y1, x1, y1);
    fb_line(x0, y0, x0, y1);
    fb_line(x1, y0, x1, y1);
}

void fb_rect_filled(int x0, int y0, int x1, int y1, bool filled)
{
    for (int32_t y = y0; y <= y1; y++)
    {
        for (int32_t x = x0; x <= x1; x++)
        {
            if (filled)
                fb_plot(x, y);
            else
                fb_plot_black(x, y);
        }
    }
}

static void draw_line_raw(int x1, int y1, int x2, int y2)
{
    int dx = abs(x2 - x1), sx = x1 < x2 ? 1 : -1;
    int dy = abs(y2 - y1), sy = y1 < y2 ? 1 : -1;
    int err = (dx > dy ? dx : -dy) / 2, e2;

    while (true)
    {
        fb_plot(x1, y1);
        if (x1 == x2 && y1 == y2)
            return;
        e2 = err;
        if (e2 > -dx)
        {
            err -= dy;
            x1 += sx;
        }
        if (e2 < dy)
        {
            err += dx;
            y1 += sy;
        }
    }
}

// Compute the bit code for a point (x, y) using the clip rectangle
// bounded diagonally by (minX, minY), and (maxX, maxY)
// ASSUME THAT maxX, minX, maxY and minY are global constants.
static OutCode compute_outcode(int x, int y)
{
    OutCode code;
    code = INSIDE; // initialised as being inside of [[clip window]]
    if (x < minX)  // to the left of clip window
        code |= LEFT;
    else if (x > maxX) // to the right of clip window
        code |= RIGHT;
    if (y < minY) // below the clip window
        code |= BOTTOM;
    else if (y > maxY) // above the clip window
        code |= TOP;
    return code;
}

// Cohen-Sutherland clipping algorithm clips a line from
// P0 = (x0, y0) to P1 = (x1, y1) against a rectangle with
// diagonal from (minX, minY) to (maxX, maxY).
void fb_line(int x0, int y0, int x1, int y1)
{
#ifdef AA
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDrawLine(renderer, x0, y0, x1, y1);
    return;
#endif
    // compute outcodes for P0, P1, and whatever point lies outside the clip rectangle
    OutCode outcode0 = compute_outcode(x0, y0);
    OutCode outcode1 = compute_outcode(x1, y1);
    bool accept = false;

    while (true)
    {
        if (!(outcode0 | outcode1))
        {
            // bitwise OR is 0: both points inside window; trivially accept and exit loop
            accept = true;
            break;
        }
        else if (outcode0 & outcode1)
        {
            // bitwise AND is not 0: both points share an outside zone (LEFT, RIGHT, TOP,
            // or BOTTOM), so both must be outside window; exit loop (accept is false)
            break;
        }
        else
        {
            // failed both tests, so calculate the line segment to clip
            // from an outside point to an intersection with clip edge
            int x, y;

            // At least one endpoint is outside the clip rectangle; pick it.
            OutCode outcodeOut = outcode0 ? outcode0 : outcode1;

            // Now find the intersection point;
            // use formulas:
            //   slope = (y1 - y0) / (x1 - x0)
            //   x = x0 + (1 / slope) * (ym - y0), where ym is minY or maxY
            //   y = y0 + slope * (xm - x0), where xm is minX or maxX
            // No need to worry about divide-by-zero because, in each case, the
            // outcode bit being tested guarantees the denominator is non-zero
            if (outcodeOut & TOP)
            { // point is above the clip window
                x = x0 + (x1 - x0) * (maxY - y0) / (y1 - y0);
                y = maxY;
            }
            else if (outcodeOut & BOTTOM)
            { // point is below the clip window
                x = x0 + (x1 - x0) * (minY - y0) / (y1 - y0);
                y = minY;
            }
            else if (outcodeOut & RIGHT)
            { // point is to the right of clip window
                y = y0 + (y1 - y0) * (maxX - x0) / (x1 - x0);
                x = maxX;
            }
            else if (outcodeOut & LEFT)
            { // point is to the left of clip window
                y = y0 + (y1 - y0) * (minX - x0) / (x1 - x0);
                x = minX;
            }
            else
            {
                // Unneeded, to avoid compiler warnings
                x = 0;
                y = 0;
            }

            // Now we move outside point to intersection point to clip
            // and get ready for next pass.
            if (outcodeOut == outcode0)
            {
                x0 = x;
                y0 = y;
                outcode0 = compute_outcode(x0, y0);
            }
            else
            {
                x1 = x;
                y1 = y;
                outcode1 = compute_outcode(x1, y1);
            }
        }
    }

    if (accept)
    {
        draw_line_raw(x0, y0, x1, y1);
    }
}

int orient2d(const int xa, const int ya, const int xb, const int yb, const int xc, const int yc)
{
    return (xb - xa) * (yc - ya) - (yb - ya) * (xc - xa);
}

void fb_triangle(int x1, int y1, int x2, int y2, int x3, int y3, bool filled)
{
#ifdef AA
    SDL_Vertex vert[3];
    vert[0].position.x = x1;
    vert[0].position.y = y1;
    vert[0].color.r = filled ? 255 : 0;
    vert[0].color.g = filled ? 255 : 0;
    vert[0].color.b = filled ? 255 : 0;
    vert[0].color.a = filled ? 255 : 0;
    vert[1].position.x = x2;
    vert[1].position.y = y2;
    vert[1].color.r = filled ? 255 : 0;
    vert[1].color.g = filled ? 255 : 0;
    vert[1].color.b = filled ? 255 : 0;
    vert[1].color.a = filled ? 255 : 0;
    vert[2].position.x = x3;
    vert[2].position.y = y3;
    vert[2].color.r = filled ? 255 : 0;
    vert[2].color.g = filled ? 255 : 0;
    vert[2].color.b = filled ? 255 : 0;
    vert[2].color.a = filled ? 255 : 0;
    SDL_RenderGeometry(renderer, NULL, vert, 3, NULL, 0);
    return;
#endif

    // Bounding box
    int minXP = min(min(x1, x2), x3);
    int maxXP = max(max(x1, x2), x3);
    int minYP = min(min(y1, y2), y3);
    int maxYP = max(max(y1, y2), y3);

    // Zero size?
    if (minXP == maxXP || minYP == maxYP)
    {
        return;
    }

    // Clip to bounds
    minXP = max(minXP, minX);
    minYP = max(minYP, minY);
    maxXP = min(maxXP, maxX);
    maxYP = min(maxYP, maxY);

    // Triangle setup
    int A01 = y1 - y2, B01 = x2 - x1;
    int A12 = y2 - y3, B12 = x3 - x2;
    int A20 = y3 - y1, B20 = x1 - x3;

    // Barycentric coordinates at minX/minY corner
    int w0_row = orient2d(x2, y2, x3, y3, minXP, minYP);
    int w1_row = orient2d(x3, y3, x1, y1, minXP, minYP);
    int w2_row = orient2d(x1, y1, x2, y2, minXP, minYP);

    // Rasterise
    for (int y = minYP; y <= maxYP; y++)
    {
        float sum = (float)w0_row + (float)w1_row + (float)w2_row;

        // Barycentric coordinates at start of row
        int w0 = w0_row;
        int w1 = w1_row;
        int w2 = w2_row;

        // Now go in from both sides and find the first pixel, then fast fill
        for (int x = minXP; x <= maxXP; x++)
        {
            bool test = (w0 | w1 | w2) >= 0;
            if (test)
            {
                if (filled)
                    fb_plot(x, y);
                else
                    fb_plot_black(x, y);
            }

            // One step to the right
            w0 += A12;
            w1 += A20;
            w2 += A01;
        }

        // One row step
        w0_row += B12;
        w1_row += B20;
        w2_row += B01;
    }
}

static void eight_way_symmetric_plot(int xc, int yc, int x, int y, bool fill)
{
    if (!fill)
    {
        fb_plot(-x + xc, -y + yc);
        fb_plot(x + xc, -y + yc);
        fb_plot(-x + xc, y + yc);
        fb_plot(x + xc, y + yc);
        fb_plot(-y + xc, -x + yc);
        fb_plot(y + xc, -x + yc);
        fb_plot(y + xc, x + yc);
        fb_plot(-y + xc, x + yc);
    }
    else
    {
        fb_line(-x + xc, -y + yc, x + xc, -y + yc);
        fb_line(-x + xc, y + yc, x + xc, y + yc);
        fb_line(-y + xc, -x + yc, y + xc, -x + yc);
        fb_line(-y + xc, x + yc, y + xc, x + yc);
    }
}

void fb_circle(int xc, int yc, int r, bool fill)
{
    int x = 0, y = r, d = 3 - (2 * r);
    eight_way_symmetric_plot(xc, yc, x, y, fill);
    while (x <= y)
    {
        if (d <= 0)
        {
            d = d + (4 * x) + 6;
        }
        else
        {
            d = d + (4 * x) - (4 * y) + 10;
            y = y - 1;
        }
        x = x + 1;
        eight_way_symmetric_plot(xc, yc, x, y, fill);
    }
}