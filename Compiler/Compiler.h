#pragma once
#include <cstddef>
#include <cstdint>
#include <map>
#include <list>
#include <string>
#include <capstone/capstone.h>
#include "Tokens.h"
#include "Words.h"
#include "../LLVM/CompilerLLVM.h"

typedef int (* start)(void);

class Compiler
{
 public:
	Compiler();
	void Compile(std::string code);
	void Run();

 private:
	void Expand();
	void DumpIR();
	void NativeInit(CompilerLLVM& llvm);
	void ProcessWord(std::string word);
	void CompileToken(Token& t);

	const size_t StackSize = 1024;
	CompilerLLVM llvm;
	std::map<std::string, Word> native_words;
	std::list<Token> tokens;
};