#include <stdio.h>
#ifdef WINDOWS
#define GL_GLEXT_PROTOTYPES 1
#include <SDL.h>
#include <SDL_opengl.h>
#else
#define GL_GLEXT_PROTOTYPES 1
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#endif
#include <math.h>

float dpi_ratio;
int desktop_screen_width, desktop_screen_height;
SDL_Window* window;
SDL_Renderer* renderer;

void beebScreen_Init()
{
	if (SDL_Init(SDL_INIT_EVERYTHING)<0) {
		printf("SDL initialization failed. SDL Error: %s\n", SDL_GetError());
	}

	float ddpi, hdpi, vdpi;
	if (SDL_GetDisplayDPI(0, &ddpi, &hdpi, &vdpi)!=0) {
		printf("Failed to obtain DPI information for display 0: %s \n", SDL_GetError());
		exit(1);
	}
#ifdef __APPLE__
	dpi_ratio = ddpi/96;
#else
	dpi_ratio = ddpi/72;
#endif
	dpi_ratio = roundf(dpi_ratio);
	//	if (dpiRatio > 1.0 && dpiRatio < 2.0) dpiRatio = 2.0;
	printf("DPI ratio: %f\n", dpi_ratio);

	SDL_DisplayMode dm;
	if (SDL_GetDesktopDisplayMode(0, &dm)!=0) {
		printf("SDL_GetDesktopDisplayMode failed: %s\n", SDL_GetError());
		exit(1);
	}
	desktop_screen_width = dm.w;
	desktop_screen_height = dm.h;
	printf("Desktop resolution: %d x %d\n", desktop_screen_width, desktop_screen_height);

	SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI);
	window = SDL_CreateWindow("Annihilation", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640*2, 256*2, window_flags);
	if (window==NULL) {
		printf("ERROR: Window could not be created. SDL_Error: %s\n", SDL_GetError());
		exit(1);
	}
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (renderer==NULL) {
		printf("ERROR: Renderer could not be created. SDL_Error: %s\n", SDL_GetError());
		exit(1);
	}
	SDL_RenderSetScale(renderer, 2.0f*dpi_ratio, 2.0f*dpi_ratio);
}

void beebScreen_Quit()
{
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

void beebScreen_Flip()
{
	SDL_RenderPresent(renderer);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
	SDL_RenderClear(renderer);
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		if (event.type==SDL_QUIT) {
			beebScreen_Quit();
			exit(1);
		}
		else if (event.type==SDL_WINDOWEVENT && event.window.event==SDL_WINDOWEVENT_CLOSE && event.window.windowID==SDL_GetWindowID(window)) {
			beebScreen_Quit();
			exit(1);
		}
	}
}

void beebScreen_SetBuffer()
{
}

void beebScreen_SetGeometry()
{
}

void beebScreen_SendPal()
{
}

void beebScreen_UseDefaultScreenBases()
{

}

