#include "Compiler.h"

void Compiler::Expand()
{
	for (auto t = tokens.begin(); t != tokens.end();)
	{
		switch (t->type)
		{
			case TokenType::WORD:
			{
				auto iter = native_words.find(t->word);
				if (iter == native_words.end())
				{
					printf("Word '%s' not found\n", t->word.c_str());
					exit(1);
				}
				auto current = t;
				t++;
				iter->second.compile(tokens, t, llvm);
				tokens.erase(current);
				break;
			}
			default:
				t++;
				break;
		}
	}
}