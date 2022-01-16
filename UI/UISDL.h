#pragma once
#include <memory>
#ifdef WINDOWS
#define GL_GLEXT_PROTOTYPES 1
#include <SDL.h>
#include <SDL_opengl.h>
#else
#define GL_GLEXT_PROTOTYPES 1
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#endif
#include "../Console/Console.h"

class UISDL {
public:
	UISDL();
	~UISDL();
	void Start();

private:
	void _CreateWindow();
	Console console;

	float dpi_ratio;
	int desktop_screen_width, desktop_screen_height;
	SDL_Window* window;
};