#include "compiler.h"

extern clist_token tokens;
extern uint8_t base;

void process_word(const char* word)
{
	size_t l = strlen(word);
	char* end;

	// Decimal
	if (word[0] == '#')
	{
		long i = strtol(&word[1], &end, 10);
		if (end != word)
		{
			clist_token_push_back(&tokens, (token){ TOKEN_PUSH_INTEGER, 0,0, 1, .v_i = i });
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
			clist_token_push_back(&tokens, (token){ TOKEN_PUSH_INTEGER, 0,0, 1, .v_i = i });
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
			clist_token_push_back(&tokens, (token){ TOKEN_PUSH_INTEGER, 0,0, 1, .v_i = i });
			return;
		}
		else
		{
			printf("Error parsing integer literal\n");
		}
	}

	// Float?
	if (strstr(word, "."))
	{
		double d = strtod(word, &end);
		if (end != word)
		{
			clist_token_push_back(&tokens, (token){ TOKEN_PUSH_FLOAT, 0, 0,1, .v_f = d });
			return;
		}
	}

	// Base?
	long i = strtol(word, &end, base);
	if (end != word)
	{
		clist_token_push_back(&tokens, (token){ TOKEN_PUSH_INTEGER, 0, 0,1, .v_i = i });
		return;
	}

	// Gotta be a word!
	clist_token_push_back(&tokens, (token){ TOKEN_WORD, 0,0,0, .word=word });
	return;
}