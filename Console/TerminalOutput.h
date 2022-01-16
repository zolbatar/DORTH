#pragma once

#include <iostream>
#include "TermColor.h"

using namespace termcolor;

static void TerminalInit(std::string section)
{
	std::cout << white << "Initialising " << bright_yellow << section << std::endl << reset;
}

static void TerminalShutdown(std::string section)
{
	std::cout << white << "Shutting down " << bright_yellow << section << std::endl << reset;
}

static void TerminalOut(std::string action, std::string section)
{
	std::cout << white << action << " " << bright_green << section << std::endl << reset;
}