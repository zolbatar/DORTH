#include <iostream>
#include "CompilerLLVM.h"
#include "../JIT/JIT.h"

void CompilerLLVM::Run()
{
	// Debug
	{
		auto p = "Out.ll";
		std::cout << "Writing " << p << "\n";
		llvm::StringRef filename_post(p);
		std::error_code EC;
		llvm::raw_fd_ostream out_post(filename_post, EC);
		Module.get()->print(out_post, nullptr);
		out_post.close();
	}

	// Optimise
	if (optimise)
	{
		OptimiseModule();
	}

	// Optimised output
	if (optimise)
	{
		auto p = "OutOptimised.ll";
		std::cout << "Writing " << p << "\n";
		llvm::StringRef filename_post(p);
		std::error_code EC;
		llvm::raw_fd_ostream out_post(filename_post, EC);
		Module.get()->print(out_post, nullptr);
		out_post.close();
	}

	// Run!
//		Strings_Clear();
	JIT jit(std::move(Module), std::move(Context));
	jit.Run(this);
//		Strings_Summary();
}