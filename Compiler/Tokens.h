#pragma once
#include <cstdint>
#include "../LLVM/CompilerLLVM.h"

enum class TokenType
{
	PUSH_INTEGER,
	PUSH_FLOAT,
	POP_R0,
	POP_R1,
	PUSH_R0,
	WORD,
	CALLNATIVE,

	ADD,
	SUBTRACT,
};

struct Token
{
	TokenType type;
	uint8_t sequence;
	uint8_t pops;
	uint8_t pushes;
	std::string word;
	llvm::FunctionCallee native;
	int64_t v_i;
	double v_f;
};