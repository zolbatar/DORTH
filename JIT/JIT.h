#pragma once
#include <memory>
#include <unordered_map>
#include "llvm/IR/IRBuilder.h"
#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/ExecutionEngine/Orc/LLJIT.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/Object/SymbolSize.h"

class JIT
{
 public:
	JIT(std::unique_ptr<llvm::Module> module, std::unique_ptr<llvm::LLVMContext> context)
		: module(std::move(module)), context(std::move(context))
	{
	}
	std::map<std::string, void*> Run(CompilerLLVM* llvm, std::list<std::string> vars);

 private:
	std::unique_ptr<llvm::Module> module = nullptr;
	std::unique_ptr<llvm::LLVMContext> context = nullptr;
	llvm::orc::LLJIT* TheJIT;

	std::map<std::string, void*> ExtractVariables(std::list<std::string> vars);
};