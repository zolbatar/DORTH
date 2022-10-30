#include "Compiler.h"

static void DumpNativeWord(Token& t, std::string desc)
{
	printf("%d/%d/%d [%s]\n", t.pushes, t.pops, t.sequence, desc.c_str());
}

void Compiler::DumpIR()
{
	for (auto t = tokens.begin(); t != tokens.end(); t++)
	{
		switch (t->type)
		{
			case TokenType::WORD:
				printf("%d/%d/%d [WORD   ]: '%s'\n", t->pushes, t->pops, t->sequence, t->word.c_str());
				break;
			case TokenType::PUSH_INTEGER:
				printf("%d/%d/%d [INTEGER]: %lld\n", t->pushes, t->pops, t->sequence, t->v_i);
				break;
			case TokenType::PUSH_FLOAT:
				printf("%d/%d/%d [FLOAT  ]: %f\n", t->pushes, t->pops, t->sequence, t->v_f);
				break;
			case TokenType::POP_R0:
				printf("%d/%d/%d [POP R0 ]\n", t->pushes, t->pops, t->sequence);
				break;
			case TokenType::POP_R1:
				printf("%d/%d/%d [POP R1 ]\n", t->pushes, t->pops, t->sequence);
				break;
			case TokenType::PUSH_R0:
				printf("%d/%d/%d [PUSH R0]\n", t->pushes, t->pops, t->sequence);
				break;
			case TokenType::CALLNATIVE:
				printf("%d/%d/%d [CALL   ]: %s\n", t->pushes, t->pops, t->sequence, t->word.c_str());
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