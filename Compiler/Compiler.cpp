#include <iostream>
#include "Compiler.h"
#include "../Library/StringLib.h"

Compiler::Compiler()
{
	llvm.SetupProfile(true, false, "Dorth", StackSize);
	NativeInit(llvm);
}

void Compiler::Compile(std::string code)
{
	tokens.clear();

	// Empty?
	trim(code);
	if (code.empty())
		return;

	// Process one token at a time
	int start = -1;
	int last_end = 0;
	int end = 0;
	for (int i = 0; i < code.length(); i++)
	{
		char c = code[i];
		if (isblank(c))
		{
			// Previous word?
			if (start != -1)
			{
				end = i;
				auto ss = code.substr(start, end - start);
				ProcessWord(ss);
				last_end = end;
				start = -1;
			}
		}
		else
		{
			if (start == -1)
				start = i;
		}
	}

	// Did we process the last word?
	if (last_end != code.length() && start != -1)
	{
		auto ss = code.substr(start, end - start);
		ProcessWord(ss);
	}

	// Compiler to intermediate
	Expand();
	DumpIR();

	// Compile to native
	for (auto& token: tokens)
		CompileToken(token);
	llvm.FinishFunc();
}

void Compiler::CompileToken(Token& t)
{
	// Decrement stack?
	if (t.pops > 0)
	{
//		llvm.DecStack(t.pops);
	}

	switch (t.type)
	{
		case TokenType::WORD:
			assert(0);
		case TokenType::PUSH_INTEGER:
			llvm.IR()->CreateStore(llvm::ConstantInt::get(llvm.TypeInt, t.v_i), llvm.StackLoc());
			break;
		case TokenType::PUSH_FLOAT:
			llvm.IR()->CreateStore(llvm::ConstantFP::get(llvm.TypeFloat, t.v_f), llvm.StackLoc());
			break;
		case TokenType::POP_R0:
			llvm.IR()->CreateStore(llvm.IR()->CreateLoad(llvm.TypeInt, llvm.StackLoc()), llvm.R0());
			break;
		case TokenType::POP_R1:
			llvm.IR()->CreateStore(llvm.IR()->CreateLoad(llvm.TypeInt, llvm.StackLoc()), llvm.R1());
			break;
		case TokenType::PUSH_R0:
			llvm.IR()->CreateStore(llvm.GetR0(), llvm.StackLoc());
			break;

		case TokenType::CALLNATIVE:
			llvm.IR()->CreateCall(t.native, { llvm.GetR0() });
			break;
		case TokenType::ADD:
			llvm.IR()->CreateStore(llvm.IR()->CreateAdd(llvm.GetR0(), llvm.GetR1()), llvm.R0());
			break;
		case TokenType::SUBTRACT:
			llvm.IR()->CreateStore(llvm.IR()->CreateSub(llvm.GetR0(), llvm.GetR1()), llvm.R0());
			break;
	}

	// Increment stack?
	if (t.pushes > 0)
	{
//		llvm.IncStack(t.pops);
	}
}

void Compiler::Run()
{
	llvm.Run();
}