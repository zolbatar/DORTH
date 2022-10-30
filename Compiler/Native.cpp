#include "Compiler.h"
#include "../LLVM/CompilerLLVM.h"
#include "../Runtime/Runtime.h"

std::string compiling_word_name;

extern "C" void __DOT(int i)
{
	char buffer[256];
	sprintf(buffer, "%d ", i);
	console_print(buffer);
}

static void COLON(std::list<Token>& tokens, std::list<Token>::const_iterator t, CompilerLLVM& llvm)
{
	state = 1;

	// Get name
	compiling_word_name = t->word;
	t++;
}

static void DOT(std::list<Token>& tokens, std::list<Token>::const_iterator t, CompilerLLVM& llvm)
{
	tokens.insert(t, Token{ TokenType::POP_R0 });
	tokens.insert(t, Token{ TokenType::CALLNATIVE,
		.native = llvm.Module->getFunction("__DOT"),
		.word="__DOT" });
}

static void DROP(std::list<Token>& tokens, std::list<Token>::const_iterator t, CompilerLLVM& llvm)
{
	tokens.insert(t, Token{ TokenType::POP_R0 });
}

static void MINUS(std::list<Token>& tokens, std::list<Token>::const_iterator t, CompilerLLVM& llvm)
{
	tokens.insert(t, Token{ TokenType::POP_R1 });
	tokens.insert(t, Token{ TokenType::POP_R0 });
	tokens.insert(t, Token{ TokenType::SUBTRACT });
	tokens.insert(t, Token{ TokenType::PUSH_R0 });
}

static void PLUS(std::list<Token>& tokens, std::list<Token>::const_iterator t, CompilerLLVM& llvm)
{
	tokens.insert(t, Token{ TokenType::POP_R1 });
	tokens.insert(t, Token{ TokenType::POP_R0 });
	tokens.insert(t, Token{ TokenType::ADD });
	tokens.insert(t, Token{ TokenType::PUSH_R0 });
}

static void SEMICOLON(std::list<Token>& tokens, std::list<Token>::const_iterator t, CompilerLLVM& llvm)
{
	state = 0;
}

static void STATE(std::list<Token>& tokens, std::list<Token>::const_iterator t, CompilerLLVM& llvm)
{
	tokens.insert(t, Token{ TokenType::PUSH_INTEGER, .v_i = state });
/*	llvm.IR()->CreateStore(llvm::ConstantInt::get(llvm.TypeInt, state), llvm.StackLoc());
	llvm.IncStack();*/
}

void Compiler::NativeInit(CompilerLLVM& llvm)
{
	llvm.Module->getOrInsertFunction("__DOT", llvm.TypeNone, llvm.TypeInt);
	native_words.emplace(std::make_pair(".", Word{ &DOT, nullptr }));
	native_words.emplace(std::make_pair("-", Word{ &MINUS, nullptr }));
	native_words.emplace(std::make_pair("+", Word{ &PLUS, nullptr }));
	native_words.emplace(std::make_pair(":", Word{ &COLON, nullptr }));
	native_words.emplace(std::make_pair(";", Word{ &SEMICOLON, nullptr }));
	native_words.emplace(std::make_pair("DROP", Word{ &DROP, nullptr }));
	native_words.emplace(std::make_pair("STATE", Word{ &STATE, nullptr }));
}
