#pragma once
#include <atomic>
#include <map>
#include <thread>
#include <vector>
#include <string>
#include <imgui.h>
#ifdef WINDOWS
#define GL_GLEXT_PROTOTYPES 1
#include <SDL.h>
#include <SDL_opengl.h>
#else
#define GL_GLEXT_PROTOTYPES 1
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#endif
#ifdef WINDOWS
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <GL/gl.h>
#endif

class Console {
public:
	void Setup(int w, int h, float dpiRatio, int sx, int sy, bool command_line, bool banked);
	void Update(ImFont* font, bool useBitmap);
	void Cls();
	void WriteCharacter(char c);
	void WriteString(std::string s);
	void SetColour(ImVec4 colour);
	void SetColour(ImU32 colour);
	void DeleteCharacter();
	void MoveCursorToNext();
	void MoveCursorToNextLine();
	void CursorOn() { cursorActive = true; }
	void CursorOff() { cursorActive = false; }
	int GetWidth() { return charsAcross; }
	void RunnerThread(std::string name);
private:
	bool banked;
	std::map<uint32_t, std::string> lines;
	bool cursorActive = true;
	ImU32 colour;
	ImU32 bgColour;
	bool command_line;
	int charsAcross, charsDown;
	float w, h;
	float size;
	int cursorX, cursorY;
	std::vector<ImWchar> screenText;
	std::vector<ImU32> screenColours;
	std::vector<ImU32> bgColours;

	size_t CalcIndex();
	void Scroll();
};
