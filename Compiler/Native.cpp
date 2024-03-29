#include "Compiler.h"
#include "../LLVM/CompilerLLVM.h"
#include "../Runtime/Runtime.h"

std::string compiling_word_name;

extern "C" void __DOT(int64_t i)
{
	char buffer[256];
	sprintf(buffer, "%lld ", i);
	console_print(buffer);
}

extern "C" void __HEXDOT(int64_t i)
{
	char buffer[256];
	sprintf(buffer, "%llx ", i);
	console_print(buffer);
}

/*
 * Interpret time
 */

static void ALLOT(CompilerLLVM& llvm)
{
	llvm.DecStack();
	auto ptr = llvm.IR()->CreateLoad(llvm.TypeInt, llvm.StackLoc());
	llvm.DecStack();
	auto elements = llvm.IR()->CreateLoad(llvm.TypeInt, llvm.StackLoc());

	// Allocate space
	auto inst = llvm::CallInst::CreateMalloc(
		llvm.IR()->GetInsertBlock(),
		llvm.TypeInt,
		llvm.TypeInt,
		llvm::ConstantInt::get(llvm.TypeInt, llvm.Module->getDataLayout().getTypeAllocSize(llvm.TypeInt)),
		elements,
		nullptr,
		"Comma");
	llvm.IR()->Insert(inst);

	// Cast to T*:
	auto val = llvm.IR()->CreatePointerCast(inst, llvm.TypePtr);
	llvm.IR()->CreateStore(ptr, val);
}

static void AT(CompilerLLVM& llvm)
{
	llvm.DecStack();
	auto ptr = llvm.IR()->CreateLoad(llvm.TypeInt, llvm.StackLoc());
	auto ptr_to = llvm.IR()->CreateIntToPtr(ptr, llvm.TypePtr);
	auto val = llvm.IR()->CreateLoad(llvm.TypeInt, ptr_to);
	llvm.IR()->CreateStore(val, llvm.StackLoc());
	llvm.IncStack();
}

static void COMMA(CompilerLLVM& llvm)
{
	// Value
	llvm.DecStack();
	auto value = llvm.IR()->CreateLoad(llvm.TypeInt, llvm.StackLoc());

	// Location
	llvm.DecStack();
	auto ptr = llvm.IR()->CreateLoad(llvm.TypeInt, llvm.StackLoc());
	auto ptr_to = llvm.IR()->CreateIntToPtr(ptr, llvm.TypePtr);

	// Allocate space
	auto inst = llvm::CallInst::CreateMalloc(
		llvm.IR()->GetInsertBlock(),
		llvm.TypeInt,
		llvm.TypeInt,
		llvm::ConstantInt::get(llvm.TypeInt, llvm.Module->getDataLayout().getTypeAllocSize(llvm.TypeInt)),
		llvm::ConstantInt::get(llvm.TypeInt, 1),
		nullptr,
		"Comma");
	llvm.IR()->Insert(inst);
	llvm.IR()->CreateStore(inst, ptr_to);

	// Save value
//	llvm.IR()->CreateStore(inst, value);
}

/*
 * Compile time
 */

static bool COLON(std::list<Token>& tokens, std::list<Token>::iterator& t, CompilerLLVM& llvm)
{
	if (state != 0)
	{
		console_print("Exception, already in non-zero compilation state.");
		exit(1);
	}
	state = 1;

	// Get name
	compiling_word_name = t->word;
	t++;
	return false;
}

static bool CREATE(std::list<Token>& tokens, std::list<Token>::iterator& t, CompilerLLVM& llvm)
{
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
	state = 0;
	return false;
}

static bool STATE(std::list<Token>& tokens, std::list<Token>::iterator& t, CompilerLLVM& llvm)
{
	tokens.insert(t, Token{ TokenType::PUSH_INTEGER, .v_i = state });
	return true;
}

void Compiler::NativeInit(CompilerLLVM& llvm)
{
	llvm.Module->getOrInsertFunction("__DOT", llvm.TypeNone, llvm.TypeInt);
	llvm.Module->getOrInsertFunction("__HEXDOT", llvm.TypeNone, llvm.TypeInt);
	native_words.emplace(std::make_pair(".", Word{ &DOT, nullptr }));
	native_words.emplace(std::make_pair("HEX.", Word{ &HEXDOT, nullptr }));
	native_words.emplace(std::make_pair("@", Word{ nullptr, &AT }));
	native_words.emplace(std::make_pair(",", Word{ nullptr, &COMMA }));
	native_words.emplace(std::make_pair("-", Word{ &MINUS, nullptr }));
	native_words.emplace(std::make_pair("+", Word{ &PLUS, nullptr }));
	native_words.emplace(std::make_pair(":", Word{ &COLON, nullptr }));
	native_words.emplace(std::make_pair(";", Word{ &SEMICOLON, nullptr }));
	native_words.emplace(std::make_pair("CREATE", Word{ &CREATE, nullptr }));
	native_words.emplace(std::make_pair("DROP", Word{ &DROP, nullptr }));
	native_words.emplace(std::make_pair("STATE", Word{ &STATE, nullptr }));
}
