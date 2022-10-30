#include "Compiler.h"

static void DumpNativeWord(Token& t, std::string desc)
{
	printf("[%s]\n", desc.c_str());
}

void Compiler::DumpIR()
{
	for (auto t = tokens.begin(); t != tokens.end(); t++)
	{
		switch (t->type)
		{
			case TokenType::WORD:
				printf("[WORD   ]: '%s'\n", t->word.c_str());
				break;
			case TokenType::PUSH_INTEGER:
				printf("[INTEGER]: %lld\n", t->v_i);
				break;
			case TokenType::PUSH_FLOAT:
				printf("[FLOAT  ]: %f\n", t->v_f);
				break;
			case TokenType::POP_R0:
				printf("[POP R0 ]\n");
				break;
			case TokenType::POP_R1:
				printf("[POP R1 ]\n");
				break;
			case TokenType::PUSH_R0:
				printf("[PUSH R0]\n");
				break;
			case TokenType::CALLNATIVE:
				printf("[CALL   ]: %s\n", t->word.c_str());
				break;

			case TokenType::ADD:
				DumpNativeWord(*t, "+      ");
				break;
			case TokenType::SUBTRACT:
				DumpNativeWord(*t, "-      ");
				break;
		}
	}
}