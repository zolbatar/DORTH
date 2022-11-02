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

	CREATEGLOBAL,
	CREATEWORD,
	ENDWORD,
	CALLNATIVE,
	ADD,
	SUBTRACT,
};

struct Token
{
	TokenType type;
	std::string word;
	llvm::FunctionCallee native;
	void (* interpret)(CompilerLLVM& llvm);
	int64_t v_i;
	double v_f;
	bool needs_compile_support = false;
};