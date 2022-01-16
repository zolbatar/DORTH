#include <chrono>
#include <cstdio>
#include <stdexcept>
#include <filesystem>
#include "Console.h"
#include "TermColor.h"
//#include "../Mode.h"
#include "../ErrorCodes.h"

using namespace termcolor;

void Console::Setup(int w, int h, float dpiRatio, int sx, int sy, bool banked) {
	this->banked = banked;
	charsAcross = sx;
	charsDown = sy;
	this->w = static_cast<float>(w)/static_cast<float>(charsAcross);
	this->h = static_cast<float>(h)/static_cast<float>(charsDown);
	this->size = fmax(this->w, this->h);
	std::cout << white << "Console resolution: " << bright_blue << charsAcross << white << "x" << bright_blue
			  << charsDown << std::endl;
	std::cout << white << "Console block size: " << bright_blue << this->w << white << "x" << bright_blue << this->h
			  << reset << std::endl;

	// Set
	screenText.clear();
	screenText.resize(charsAcross*charsDown);
	screenColours.clear();
	screenColours.resize(charsAcross*charsDown);
	bgColours.clear();
	bgColours.resize(charsAcross*charsDown);
	Cls();
	SetColour(ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
}

void Console::SetColour(ImVec4 colour) {
	this->colour = ImGui::ColorConvertFloat4ToU32(colour);
}

void Console::SetColour(ImU32 colour) {
	this->colour = colour;
}

void Console::Update(ImFont* font) {
	auto wl = ImGui::GetWindowDrawList();
	auto desc = font->Descent/font->FontSize*size;
	auto asc = font->Ascent/font->FontSize*size;

	auto i = 0;
	for (auto y = 0; y<charsDown; y++) {
		for (auto x = 0; x<charsAcross; x++) {
			wl->AddRectFilled(ImVec2(x*w, (y*h)+desc+2), ImVec2((x+1)*w, (y*h)+asc), bgColours[i]);
			font->RenderChar(wl, size, ImVec2(x*w, (y*h)+desc), screenColours[i], screenText[i]);
			i++;
		}
	}

	// Do we need a cursor?
	auto now = std::chrono::duration_cast<std::chrono::milliseconds>(
			std::chrono::system_clock::now().time_since_epoch()).count();
	now /= 150.0;
	if (now%2==1 && cursorActive) {
		wl->AddRectFilled(ImVec2(cursorX*w, (cursorY*h)+desc), ImVec2((cursorX+1)*w, (cursorY*h)+asc), 0xFFFFFFFF);
	}
}

void Console::Cls() {
	for (size_t i = 0; i<charsAcross*charsDown; i++) {
		screenText[i] = 'A';
		screenColours[i] = 0x00000000;
		bgColours[i] = 0x00000000;
	}
	cursorX = 0;
	cursorY = 0;
}

size_t Console::CalcIndex() {
	return (cursorY*charsAcross)+cursorX;
}

void Console::DeleteCharacter() {
	cursorX--;
	if (cursorX<0) {
		cursorY--;
		cursorX = charsAcross-1;
	}
	auto i = CalcIndex();
	screenText[i] = ' ';
	screenColours[i] = 0x00000000;
	bgColours[i] = 0x00000000;
}

void Console::WriteCharacter(char c) {
	if (c==13) {
		MoveCursorToNextLine();
	}
	else if (c==10) {
		// Absorb
	}
	else {
		auto i = CalcIndex();
		screenText[i] = c;
		screenColours[i] = colour;
		bgColours[i] = bgColour;
		MoveCursorToNext();
	}
}

void Console::MoveCursorToNext() {
	// Next position
	cursorX++;

	// Next line?
	if (cursorX==charsAcross) {
		MoveCursorToNextLine();
	}
}

void Console::MoveCursorToNextLine() {
	cursorX = 0;
	cursorY++;

	// Scroll?
	if (cursorY==charsDown) {
		Scroll();
	}
}

void Console::Scroll() {
	cursorY--;

	for (size_t i = 0; i<charsAcross*(charsDown-1); i++) {
		screenText[i] = screenText[i+charsAcross];
		screenColours[i] = screenColours[i+charsAcross];
		bgColours[i] = bgColours[i+charsAcross];
	}

	auto e = charsAcross*charsDown;
	for (size_t i = 0; i<charsAcross; i++) {
		screenText[e-i-1] = ' ';
		screenColours[e-i-1] = 0x00000000;
		bgColours[e-i-1] = 0x00000000;
	}
}

void Console::WriteString(std::string s) {
	for (auto c = s.begin(); c!=s.end(); ++c) {
		WriteCharacter(*c);
	}
}