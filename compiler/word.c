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
			clist_token_push_back(&tokens, (token){ TOKEN_PUSH_INTEGER, 0, .v_i = i });
			clist_token_push_back(&tokens, (token){ TOKEN_INC_SP, 1 });
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
			clist_token_push_back(&tokens, (token){ TOKEN_PUSH_INTEGER, 0, .v_i = i });
			clist_token_push_back(&tokens, (token){ TOKEN_INC_SP, 1 });
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
			clist_token_push_back(&tokens, (token){ TOKEN_PUSH_INTEGER, 0, .v_i = i });
			clist_token_push_back(&tokens, (token){ TOKEN_INC_SP, 1 });
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
			clist_token_push_back(&tokens, (token){ TOKEN_PUSH_FLOAT, 0, .v_f = d });
			clist_token_push_back(&tokens, (token){ TOKEN_INC_SP, 1 });
			return;
		}
	}

	// Base?
	long i = strtol(word, &end, base);
	if (end != word)
	{
		clist_token_push_back(&tokens, (token){ TOKEN_PUSH_INTEGER, 0, .v_i = i });
		clist_token_push_back(&tokens, (token){ TOKEN_INC_SP, 1 });
		return;
	}

	// Gotta be a word!
	clist_token_push_back(&tokens, (token){ TOKEN_WORD, 0, .word=word });
	return;
}