#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "compiler.h"

void process_word(const char* word);

void compile(const char* source)
{
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
}

void process_word(const char* word)
{
	token_type t = TOKEN_NONE;
	size_t l = strlen(word);

	// Try parsing as double first
	

	for (unsigned i = 0; i < l; i++)
	{
		char c = word[i];

		// A standard state machine
		switch (t)
		{
		case TOKEN_NONE:
			break;
		case TOKEN_WORD:
			break;
		case TOKEN_INTEGER:
			break;
		case TOKEN_FLOAT:
			break;
		}
	}
//	printf("Word: %s\n", word);
}