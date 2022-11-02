#include <iostream>
#include "Compiler.h"

void Compiler::Expand()
{
	for (auto t = tokens.begin(); t != tokens.end();)
	{
		ExpandToken(t);
	}
}

void Compiler::ExpandToken(std::list<Token>::iterator& t)
{
//	std::cout << t->word << std::endl;
	switch (t->type)
	{
		case TokenType::WORD:
		{
			auto iter = native_words.find(t->word);
			if (iter == native_words.end())
			{
				// No word logic found
				t++;
			}
			else
			{
				auto current = t;
				t++;
				if (iter->second.compile != nullptr)
				{
					if (iter->second.compile(tokens, t, llvm))
						tokens.erase(current);
				}
				else
				{
					current->interpret = iter->second.interpret;
				}
			}
			break;
		}
		default:
			t++;
			break;
	}
}