#pragma once
#include <memory>
#include "llvm/IR/IRBuilder.h"

class JIT {
public:
	JIT(std::unique_ptr<llvm::Module> module, std::unique_ptr<llvm::LLVMContext> context)
			:module(std::move(module)), context(std::move(context)) { }
	void run();
private:
	std::unique_ptr<llvm::Module> module;
	std::unique_ptr<llvm::LLVMContext> context;
};