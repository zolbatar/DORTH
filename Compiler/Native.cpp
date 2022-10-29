#include "Compiler.h"
#include "../Runtime/Runtime.h"

void __DOT(int i)
{
	char buffer[256];
	sprintf(buffer, "%d ", i);
	console_print(buffer);
}

static void DOT(std::list<Token>& tokens, std::list<Token>::const_iterator t)
{
	tokens.insert(t, Token{ TokenType::CALLNATIVE, 0, 0, 0, .native = (void*)&__DOT });
	tokens.insert(t, Token{ TokenType::POP_R0, 0, 1, 0 });
}

static void MINUS(std::list<Token>& tokens, std::list<Token>::const_iterator t)
{
	tokens.insert(t, Token{ TokenType::PUSH_R0, 0, 0, 1 });
	tokens.insert(t, Token{ TokenType::SUBTRACT });
	tokens.insert(t, Token{ TokenType::POP_R0, 0, 1, 0 });
	tokens.insert(t, Token{ TokenType::POP_R1, 0, 1, 0 });
}

static void PLUS(std::list<Token>& tokens, std::list<Token>::const_iterator t)
{
	tokens.insert(t, Token{ TokenType::PUSH_R0, 0, 0, 1 });
	tokens.insert(t, Token{ TokenType::ADD });
	tokens.insert(t, Token{ TokenType::POP_R0, 0, 1, 0 });
	tokens.insert(t, Token{ TokenType::POP_R1, 0, 1, 0 });
}

static void DROP(std::list<Token>& tokens, std::list<Token>::const_iterator t)
{
	tokens.insert(t, Token{ TokenType::POP_R0, 0, 1, 0 });
}

void Compiler::NativeInit()
{
	native_words.emplace(std::make_pair(".", Word{ &DOT, NULL }));
/*	cmap_str_emplace(&words, ".", (word){ &native_DOT, NULL });
	cmap_str_emplace(&words, "-", (word){ &native_MINUS, NULL });
	cmap_str_emplace(&words, "+", (word){ &native_PLUS, NULL });
	cmap_str_emplace(&words, "DROP", (word){ &native_DROP, NULL });*/
}
