#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "compiler.h"
#include <regex.h>

clist_token tokens;
regex_t reg_float;
regex_t reg_base;
regex_t reg_decnum;
regex_t reg_hexnum;
regex_t reg_binnum;

void process_word(const char* word);

void compiler_init()
{
	if (regcomp(&reg_float, "[-+]?[0-9]+[.][0-9]+([E][-+]?[0-9]+)?", REG_EXTENDED) != 0)
	{
		printf("Error compiling regex for float\n");
		exit(1);
	}
	if (regcomp(&reg_base, "[-]?[0-9a-zA-Z]+", REG_EXTENDED) != 0)
	{
		printf("Error compiling regex for base\n");
		exit(1);
	}
	if (regcomp(&reg_decnum, "#[-]?[0-9]+", REG_EXTENDED) != 0)
	{
		printf("Error compiling regex for decimal\n");
		exit(1);
	}
	if (regcomp(&reg_hexnum, "$[-0]?[0-9a-fA-F]+", REG_EXTENDED) != 0)
	{
		printf("Error compiling regex for hexadecimal\n");
		exit(1);
	}
	if (regcomp(&reg_binnum, "[-]?[0-1]+", REG_EXTENDED) != 0)
	{
		printf("Error compiling regex for binary\n");
		exit(1);
	}
}

void compile(const char* source)
{
	tokens = clist_token_init();
	size_t l = strlen(source);
	if (l == 0)
		return;

	// Process one token at a time
	int start = -1;
	int last_end = 0;
	int end = 0;
	for (int i = 0; i < l; i++)
	{
		char c = source[i];
		if (isblank(c))
		{
			// Previous word?
			if (start != -1)
			{
				end = i;
				char word[end - start + 1];
				strncpy(word, source + start, end - start);
				word[end - start] = 0;
//				printf("Word at %d-%d: %s\n", start, end, (const char*)&word);
				last_end = end;
				start = -1;
				process_word((const char*)&word);
			}
			else
			{
				// Absorb
			}
		}
		else
		{
			if (start == -1)
				start = i;
		}
	}

	// Did we process the last word?
	if (last_end != l && start != -1)
	{
		end = l;
		char word[end - start + 1];
		strncpy(word, source + start, end - start);
		word[end - start] = 0;
//		printf("Word at %d-%d: %s\n", start, end, (const char*)&word);
		process_word((const char*)&word);
	}

	// Dump out list of tokens
	c_foreach (t, clist_token, tokens)
	{
		printf("..\n");
	}
}

void process_word(const char* word)
{
	size_t l = strlen(word);

	if (regexec(&reg_float, word, 0, NULL, 0) == 0)
	{
		char* end;
		double d = strtod(word, &end);
		if (end != word)
		{
			token t;
			t.type = TOKEN_FLOAT;
			t.v_f = d;
			clist_token_push_back(&tokens, t);
			return;
		}
		else
		{
			printf("Parsing double error\n");
		}
	}

	if (regexec(&reg_decnum, word, 0, NULL, 0) == 0)
	{
		char* end;
		double d = strtod(word, &end);
		if (end != word)
		{
			token t;
			t.type = TOKEN_INTEGER;
			t.v_f = d;
			clist_token_push_back(&tokens, t);
			return;
		}
		else
		{
			printf("Parsing integer error\n");
		}
	}
}