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
			token t;
			t.type = TOKEN_INTEGER;
			t.sequence = 0;
			t.v_i = i;
			clist_token_push_back(&tokens, t);
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
			token t;
			t.type = TOKEN_INTEGER;
			t.sequence = 0;
			t.v_i = i;
			clist_token_push_back(&tokens, t);
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
			token t;
			t.type = TOKEN_INTEGER;
			t.sequence = 0;
			t.v_i = i;
			clist_token_push_back(&tokens, t);
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
			token t;
			t.type = TOKEN_FLOAT;
			t.sequence = 0;
			t.v_f = d;
			clist_token_push_back(&tokens, t);
			return;
		}
	}

	// Base?
	long i = strtol(word, &end, base);
	if (end != word)
	{
		token t;
		t.type = TOKEN_INTEGER;
		t.sequence = 0;
		t.v_i = i;
		clist_token_push_back(&tokens, t);
		return;
	}

	// Gotta be a word!
	token t;
	t.type = TOKEN_WORD;
	t.sequence = 0;
	t.word = word;
	clist_token_push_back(&tokens, t);
	return;
}