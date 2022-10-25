#pragma once
#include <memory>
#include <vector>
#include <fstream>
#include <sstream>

#ifdef WINDOWS
#define GL_GLEXT_PROTOTYPES 1
#include <SDL.h>
#include <SDL_opengl.h>
#else
#define GL_GLEXT_PROTOTYPES 1

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#endif

#include "imgui.h"
#include "../../ImGuiBindings/imgui_impl_sdl.h"
#include "../../ImGuiBindings/imgui_impl_opengl3.h"

enum class Mode
{
	CLASSIC,
	BANKED
};

class UISDL
{
 public:
	UISDL();
	~UISDL();

	void Start();
	void ChangeMode(int64_t w, int64_t h, Mode mode);
	void SetFGColour(ImU32 colour);
	void SetBGColour(ImU32 colour);
	void Cls();
	void Flip(bool userSpecified);
	void Line(float x1, float y1, float x2, float y2);
	void LineWidth(float x1, float y1, float x2, float y2, float w);
	void Rectangle(float x1, float y1, float x2, float y2);
	void FilledRectangle(float x1, float y1, float x2, float y2);
	void Circle(float x, float y, float r);
	void CircleWidth(float x, float y, float r, float w);
	void FilledCircle(float x, float y, float r);
	void Triangle(float x1, float y1, float x2, float y2, float x3, float y3);
	void FilledTriangle(float x1, float y1, float x2, float y2, float x3, float y3);
	void ShadedTriangle(float x1, float y1, float x2, float y2, float x3, float y3, uint32_t colour1, uint32_t colour2, uint32_t colour3, bool flat);
	void DrawText(ImFont* font, float size, float x, float y, float w, float h, std::string text);
	void Plot(float x, float y);
	void Clip(float x1, float y1, float x2, float y2);
	void ClipOff();

	int64_t GetScreenWidth()
	{
		return desktop_screen_width;
	}

	int64_t GetScreenHeight()
	{
		return desktop_screen_height;
	}

	float GetDPIRatio()
	{
		return dpi_ratio;
	}

	float GetFontSize()
	{
		return font_size;
	}

	ImU32 GetFGColour()
	{
		return fgColour;
	}

 private:
	void _CreateWindow();
	bool LoadTextureFromFile(const char* filename, GLuint* outTexture, int* outWidth, int* outHeight);

	const float font_size = 20.0f;
	const int console_x_size = 12;
	const int console_y_size = 24;
	ImU32 fgColour;
	ImU32 bgColour;
	std::atomic_bool flip_requested = false;
	const bool fullscreen = false;
	float dpi_ratio;
	int desktop_screen_width, desktop_screen_height;
	SDL_Window* window;
	ImFont* fontMono;
	Mode mode = Mode::CLASSIC;
};