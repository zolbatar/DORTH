#include <iostream>
#include "../LLVM/CompilerLLVM.h"
#include "JIT.h"
#include "llvm/ExecutionEngine/Orc/LLJIT.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/Object/SymbolSize.h"

void JIT::Run(CompilerLLVM* llvm)
{
	// Create an LLJIT instance.
	/*auto jit = llvm::orc::LLLazyJITBuilder()
			.setNumCompileThreads(4)
			.setLazyCompileFailureAddr(ea)
			.create();*/
	auto jit = llvm::orc::LLJITBuilder().setNumCompileThreads(4).create();
	if (!jit)
	{
		std::cout << "Can't create JIT\n";
		exit(1);
	}
	auto JIT = jit->get();

	// Add compiled module (with linked in library)
	auto error = JIT->addIRModule(llvm::orc::ThreadSafeModule(std::move(module), std::move(context)));
	if (error)
	{
		std::cout << "Error adding IR module\n";
		exit(1);
	}

	// Execute
	auto dl = JIT->getDataLayout();
	auto generator = JIT->getMainJITDylib()
		.addGenerator(std::move(llvm::orc::DynamicLibrarySearchGenerator::GetForCurrentProcess(dl.getGlobalPrefix())
			.get()));

	// Try and find the Implicit function, if fails then compilation likely failed
	try
	{
		auto proc_start = JIT->lookup("Implicit");
		auto entry = llvm::jitTargetAddressToFunction<void (*)()>(proc_start->getValue());
		llvm->Disassemble((void*)entry, 50, (size_t)entry);
		entry();
	}
	catch (const std::exception& ex)
	{
		std::cout << "JIT execution failed, likely to due to failed compilation. This is an internal error.\n";
		std::cout << ex.what() << std::endl;
		exit(1);
	}
}