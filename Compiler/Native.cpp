#include "Compiler.h"
#include "../LLVM/CompilerLLVM.h"
#include "../Runtime/Runtime.h"

extern "C" void __DOT(int i)
{
	char buffer[256];
	sprintf(buffer, "%d ", i);
	console_print(buffer);
}

static void DOT(std::list<Token>& tokens, std::list<Token>::const_iterator t, CompilerLLVM& llvm)
{
	tokens.insert(t, Token{ TokenType::POP_R0, 0, 1, 0 });
	tokens.insert(t, Token{ TokenType::CALLNATIVE, 0, 0, 0,
		.native = llvm.Module->getFunction("__DOT"),
		.word="__DOT" });
}

static void MINUS(std::list<Token>& tokens, std::list<Token>::const_iterator t, CompilerLLVM& llvm)
{
	tokens.insert(t, Token{ TokenType::POP_R1, 0, 1, 0 });
	tokens.insert(t, Token{ TokenType::POP_R0, 0, 1, 0 });
	tokens.insert(t, Token{ TokenType::SUBTRACT });
	tokens.insert(t, Token{ TokenType::PUSH_R0, 0, 0, 1 });
}

static void PLUS(std::list<Token>& tokens, std::list<Token>::const_iterator t, CompilerLLVM& llvm)
{
	tokens.insert(t, Token{ TokenType::POP_R1, 0, 1, 0 });
	tokens.insert(t, Token{ TokenType::POP_R0, 0, 1, 0 });
	tokens.insert(t, Token{ TokenType::ADD });
	tokens.insert(t, Token{ TokenType::PUSH_R0, 0, 0, 1 });
}

static void DROP(std::list<Token>& tokens, std::list<Token>::const_iterator t, CompilerLLVM& llvm)
{
	tokens.insert(t, Token{ TokenType::POP_R0, 0, 1, 0 });
}

void Compiler::NativeInit(CompilerLLVM& llvm)
{
	llvm.Module->getOrInsertFunction("__DOT", llvm.TypeNone, llvm.TypeInt);
	native_words.emplace(std::make_pair(".", Word{ &DOT, NULL }));
	native_words.emplace(std::make_pair("-", Word{ &MINUS, NULL }));
	native_words.emplace(std::make_pair("+", Word{ &PLUS, NULL }));
	native_words.emplace(std::make_pair("DROP", Word{ &DROP, NULL }));
}
