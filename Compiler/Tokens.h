#pragma once
#include <cstdint>

enum class TokenType
{
	PUSH_INTEGER,
	PUSH_FLOAT,
	POP_R0,
	POP_R1,
	PUSH_R0,
	WORD,
	CALLNATIVE,

	// Optimised tokens
	INTEGER_TO_R0,
	INTEGER_TO_R1,

	ADD,
	SUBTRACT,
};

struct Token
{
	TokenType type;
	uint8_t sequence;
	uint8_t pops;
	uint8_t pushes;
	union
	{
		const char* word;
		void* native;
		int v_i;
		double v_f;
	};
};