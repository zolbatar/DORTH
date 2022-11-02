#include <iostream>
#include "Compiler.h"
#include "../Library/StringLib.h"

Compiler::Compiler()
{
	llvm.SetupProfile(true, false, "Dorth", StackSize, DataSize);
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
	for (int i = 0; i < code.length(); i++)
	{
		char c = code[i];
		if (isblank(c))
		{
			// Previous word?
			if (start != -1)
			{
				auto ss = code.substr(start, i - start);
				ProcessWord(ss);
				last_end = i;
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
		auto ss = code.substr(start, code.length() - start);
		ProcessWord(ss);
	}

	// Compiler to intermediate
	Expand();
	DumpIR();

	// Compile to native
	variables.clear();
	for (auto& token: tokens)
		CompileToken(token);
	llvm.FinishFunc();
}

void Compiler::CompileToken(Token& t)
{
	switch (t.type)
	{
		case TokenType::WORD:
			if (t.interpret)
			{
				t.interpret(llvm);
			}
			else
			{
				llvm::GlobalVariable* glob = llvm.GetGlobal(t.word);
				if (glob)
				{
					llvm.IR()->CreateStore(llvm.IR()->CreateLoad(llvm.TypePtr, glob), llvm.StackLoc());
					llvm.IncStack();
				}
				else
				{
					std::cout << "Word '" << t.word << "' not found" << std::endl;
				}
			}
			break;
		case TokenType::PUSH_INTEGER:
			llvm.IR()->CreateStore(llvm::ConstantInt::get(llvm.TypeInt, t.v_i), llvm.StackLoc());
			llvm.IncStack();
			break;
		case TokenType::PUSH_FLOAT:
			llvm.IR()->CreateStore(llvm::ConstantFP::get(llvm.TypeFloat, t.v_f), llvm.StackLoc());
			llvm.IncStack();
			break;
		case TokenType::PUSH_R0:
			llvm.IR()->CreateStore(llvm.GetR0(), llvm.StackLoc());
			llvm.IncStack();
			break;
		case TokenType::POP_R0:
			llvm.DecStack();
			llvm.IR()->CreateStore(llvm.IR()->CreateLoad(llvm.TypeInt, llvm.StackLoc()), llvm.R0());
			break;
		case TokenType::POP_R1:
			llvm.DecStack();
			llvm.IR()->CreateStore(llvm.IR()->CreateLoad(llvm.TypeInt, llvm.StackLoc()), llvm.R1());
			break;

		case TokenType::CALLNATIVE:
			llvm.IR()->CreateCall(t.native, { llvm.GetR0() });
			break;
		case TokenType::CREATEGLOBAL:
			variables.push_back(t.word);
			llvm.IR()->CreateStore(llvm.CreateGlobal(t.word), llvm.StackLoc());
			llvm.IncStack();
			break;
		case TokenType::ADD:
			llvm.IR()->CreateStore(llvm.IR()->CreateAdd(llvm.GetR0(), llvm.GetR1()), llvm.R0());
			break;
		case TokenType::SUBTRACT:
			llvm.IR()->CreateStore(llvm.IR()->CreateSub(llvm.GetR0(), llvm.GetR1()), llvm.R0());
			break;
	}
}

void Compiler::Run()
{
	llvm.Run(variables);
}