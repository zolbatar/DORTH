#include "Compiler.h"
#include "../Runtime/Runtime.h"

void Compiler::ProcessWord(std::string word)
{
	char* end;

	// Decimal
	if (word[0] == '#')
	{
		long i = strtol(&word[1], &end, 10);
		if (end != word)
		{
			tokens.emplace_back(Token{ TokenType::PUSH_INTEGER, 0, 0, 1, .v_i = i });
			return;
		}
		else
		{
			printf("Error parsing integer literal\n");
		}
	}

	// Hexadecimal
	if (word[0] == '$')
	{
		long i = strtol(&word[1], &end, 16);
		if (end != word)
		{
			tokens.emplace_back(Token{ TokenType::PUSH_INTEGER, 0, 0, 1, .v_i = i });
			return;
		}
		else
		{
			printf("Error parsing integer literal\n");
		}
	}

	// Binary
	if (word[0] == '%')
	{
		long i = strtol(&word[1], &end, 2);
		if (end != word)
		{
			tokens.emplace_back(Token{ TokenType::PUSH_INTEGER, 0, 0, 1, .v_i = i });
			return;
		}
		else
		{
			printf("Error parsing integer literal\n");
		}
	}

	// Float?
	if (word.find(".") != std::string::npos)
	{
		double d = std::strtod(word.c_str(), &end);
		if (end != word)
		{
			tokens.emplace_back(Token{ TokenType::PUSH_FLOAT, 0, 0, 1, .v_f = d });
			return;
		}
	}

	// Base?
	long i = std::strtol(word.c_str(), &end, base);
	if (end != word)
	{
		tokens.emplace_back(Token{ TokenType::PUSH_INTEGER, 0, 0, 1, .v_i = i });
		return;
	}

	// Gotta be a word!
	tokens.emplace_back(Token{ TokenType::WORD, 0, 0, 1, .word = word });
	return;
}