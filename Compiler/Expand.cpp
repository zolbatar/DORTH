#include "Compiler.h"

void Compiler::Expand()
{
	for (auto t = tokens.begin(); t != tokens.end();)
	{
		ExpandToken(t);
	}
}

void Compiler::ExpandToken(std::list<Token>::iterator t)
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
			if (iter->second.compile != nullptr)
			{
				iter->second.compile(tokens, t, llvm);
				tokens.erase(current);
			}
			else
			{
				current->interpret = iter->second.interpret;
			}
			break;
		}
		default:
			t++;
			break;
	}
}