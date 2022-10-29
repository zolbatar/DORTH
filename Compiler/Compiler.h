#pragma once
#include <cstddef>
#include <cstdint>
#include <map>
#include <list>
#include <string>
#include <capstone/capstone.h>
#include "Tokens.h"
#include "Words.h"

typedef int (* start)(void);

class Compiler
{
 public:
	Compiler();
	void Compile(std::string code);

 private:
	void NativeInit();
	void SetupCapstone();
	void Disassemble(start exec, size_t sz, size_t address);

	std::map<std::string, Word> native_words;
};