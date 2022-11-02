#include <iostream>
#include "Compiler.h"

std::string compiling_word_name;
Token* current_word = nullptr;
std::map<std::string, std::list<Token>> words;
std::set<std::string> words_funcs;

extern "C" void __DOT(int64_t i)
{
	char buffer[256];
	snprintf(buffer, 256, "%lld ", i);
	console_print(buffer);
}

extern "C" void __HEXDOT(int64_t i)
{
	char buffer[256];
	snprintf(buffer, 256, "%llx ", i);
	console_print(buffer);
}

/*
 * Interpret time
 */

static void ALLOT(CompilerLLVM& llvm)
{
	// Number of elements
	llvm.DecStack();
	auto elements = llvm.IR()->CreateLoad(llvm.TypeInt, llvm.StackLoc());

	// Move pointer
	llvm.IncDP(elements);
}

static void AT(CompilerLLVM& llvm)
{
	llvm.DecStack();
	auto ptr = llvm.IR()->CreateLoad(llvm.TypePtr, llvm.StackLoc());
//	auto ptr_to = llvm.IR()->CreateIntToPtr(ptr, llvm.TypePtr);
	auto val = llvm.IR()->CreateLoad(llvm.TypeInt, ptr);
	llvm.IR()->CreateStore(val, llvm.StackLoc());
	llvm.IncStack();
}

static void COMMA(CompilerLLVM& llvm)
{
	// Value
	llvm.DecStack();
	auto value = llvm.IR()->CreateLoad(llvm.TypeInt, llvm.StackLoc());

	// Store and move pointer
	llvm.IR()->CreateStore(value, llvm.DataLoc());
	llvm.IncDP(llvm::ConstantInt::get(llvm.TypeInt, 1));
}

static void HERE(CompilerLLVM& llvm)
{
	auto dp = llvm.DataLoc();
	llvm.IR()->CreateStore(dp, llvm.StackLoc());
	llvm.IncStack();
}

/*
 * Compile time
 */

static bool COLON(std::list<Token>& tokens, std::list<Token>::iterator& t, CompilerLLVM& llvm)
{
	if (state != CompilerState::NORMAL)
	{
		console_print("Exception, already in non-zero compilation state.");
		exit(1);
	}
	state = CompilerState::COMPILATION;

	current_word = &*tokens.insert(t, Token{ TokenType::CREATEWORD, .word = t->word });
	auto current = t;
	t++;
	tokens.erase(current);
	return true;
}

static bool CREATE(std::list<Token>& tokens, std::list<Token>::iterator& t, CompilerLLVM& llvm)
{
	if (state != CompilerState::NORMAL)
	{
		current_word->needs_compile_support = true;
		return false;
	}
	tokens.insert(t, Token{ TokenType::CREATEGLOBAL, .word = t->word });
	auto current = t;
	t++;
	tokens.erase(current);
	return true;
}

static bool DOT(std::list<Token>& tokens, std::list<Token>::iterator& t, CompilerLLVM& llvm)
{
	tokens.insert(t, Token{ TokenType::POP_R0 });
	tokens.insert(t, Token{ TokenType::CALLNATIVE,
		.native = llvm.Module->getFunction("__DOT"),
		.word="__DOT" });
	return true;
}

static bool HEXDOT(std::list<Token>& tokens, std::list<Token>::iterator& t, CompilerLLVM& llvm)
{
	tokens.insert(t, Token{ TokenType::POP_R0 });
	tokens.insert(t, Token{ TokenType::CALLNATIVE,
		.native = llvm.Module->getFunction("__HEXDOT"),
		.word="__DOT" });
	return true;
}

static bool DROP(std::list<Token>& tokens, std::list<Token>::iterator& t, CompilerLLVM& llvm)
{
	tokens.insert(t, Token{ TokenType::POP_R0 });
	return true;
}

static bool MINUS(std::list<Token>& tokens, std::list<Token>::iterator& t, CompilerLLVM& llvm)
{
	tokens.insert(t, Token{ TokenType::POP_R1 });
	tokens.insert(t, Token{ TokenType::POP_R0 });
	tokens.insert(t, Token{ TokenType::SUBTRACT });
	tokens.insert(t, Token{ TokenType::PUSH_R0 });
	return true;
}

static bool PLUS(std::list<Token>& tokens, std::list<Token>::iterator& t, CompilerLLVM& llvm)
{
	tokens.insert(t, Token{ TokenType::POP_R1 });
	tokens.insert(t, Token{ TokenType::POP_R0 });
	tokens.insert(t, Token{ TokenType::ADD });
	tokens.insert(t, Token{ TokenType::PUSH_R0 });
	return true;
}

static bool SEMICOLON(std::list<Token>& tokens, std::list<Token>::iterator& t, CompilerLLVM& llvm)
{
	//tokens.insert(t, Token{ TokenType::ENDWORD });
	state = CompilerState::NORMAL;

	auto name = current_word->word;
	if (current_word->needs_compile_support)
	{
		// Find where this word starts
		auto iter = tokens.end();
		for (auto i = tokens.begin(); i != tokens.end(); ++i)
		{
			if (&*i == current_word)
			{
				iter = i;
				break;
			}
		}

		// And ends
		auto end_iter = iter;
		end_iter++;
		while (end_iter != t)
		{
			end_iter++;
		}
		end_iter--;

		// Create new word list
		std::list<Token> word;
		word.splice(word.begin(), tokens, iter, end_iter);

		// And move to "words" map
		words.insert(std::make_pair(name, std::move(word)));
	}
	else
	{
		tokens.insert(t, Token{ TokenType::ENDWORD });

		// Create to call
		words_funcs.insert(name);
	}

	return true;
}

static bool STATE(std::list<Token>& tokens, std::list<Token>::iterator& t, CompilerLLVM& llvm)
{
	switch (state)
	{
		case CompilerState::NORMAL:
			tokens.insert(t, Token{ TokenType::PUSH_INTEGER, .v_i = 0 });
			break;
		default:
			tokens.insert(t, Token{ TokenType::PUSH_INTEGER, .v_i = 1 });
			break;
	}
	return true;
}

void Compiler::NativeInit(CompilerLLVM& llvm)
{
	llvm.Module->getOrInsertFunction("__DOT", llvm.TypeNone, llvm.TypeInt);
	llvm.Module->getOrInsertFunction("__HEXDOT", llvm.TypeNone, llvm.TypeInt);
	native_words.emplace(std::make_pair(".", Word{ .compile=&DOT }));
	native_words.emplace(std::make_pair("HEX.", Word{ .compile=&HEXDOT }));
	native_words.emplace(std::make_pair("@", Word{ .interpret=&AT }));
	native_words.emplace(std::make_pair(",", Word{ .interpret=&COMMA }));
	native_words.emplace(std::make_pair("-", Word{ .compile=&MINUS }));
	native_words.emplace(std::make_pair("+", Word{ .compile=&PLUS }));
	native_words.emplace(std::make_pair(":", Word{ .compile=&COLON }));
	native_words.emplace(std::make_pair(";", Word{ .compile=&SEMICOLON }));
	native_words.emplace(std::make_pair("CREATE", Word{ .compile=&CREATE }));
	native_words.emplace(std::make_pair("DROP", Word{ .compile=&DROP }));
	native_words.emplace(std::make_pair("HERE", Word{ .interpret= &HERE }));
	native_words.emplace(std::make_pair("STATE", Word{ .compile=&STATE }));
}
