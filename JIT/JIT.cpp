#include <iostream>
#include "../LLVM/CompilerLLVM.h"
#include "JIT.h"

std::map<std::string, void*> JIT::Run(CompilerLLVM* llvm, std::list<std::string> vars)
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
	TheJIT = jit->get();

	// Add compiled module (with linked in library)
	auto error = TheJIT->addIRModule(llvm::orc::ThreadSafeModule(std::move(module), std::move(context)));
	if (error)
	{
		std::cout << "Error adding IR module\n";
		exit(1);
	}

	// Execute
	auto dl = TheJIT->getDataLayout();
	auto generator = TheJIT->getMainJITDylib()
		.addGenerator(std::move(llvm::orc::DynamicLibrarySearchGenerator::GetForCurrentProcess(dl.getGlobalPrefix())
			.get()));

	// Try and find the Implicit function, if fails then compilation likely failed
	try
	{
		auto proc_start = TheJIT->lookup("Implicit");
		auto entry = llvm::jitTargetAddressToFunction<void (*)()>(proc_start->getValue());
//		llvm->Disassemble((void*)entry, 50, (size_t)entry);
		entry();
		//llvm::raw_ostream& output = llvm::outs();
		//TheJIT->getExecutionSession().dump(output);
		return ExtractVariables(vars);
	}
	catch (const std::exception& ex)
	{
		std::cout << "JIT execution failed, likely to due to failed compilation. This is an internal error.\n";
		std::cout << ex.what() << std::endl;
		exit(1);
	}
}

std::map<std::string, void*> JIT::ExtractVariables(std::list<std::string> vars)
{
	std::map<std::string, void*> ret;

	// Stack
	{
		ret.insert(std::make_pair(
			"~STACK",
			(int64_t**)TheJIT->lookup("Stack").get().getValue()));
		ret.insert(std::make_pair(
			"~STACK-PTR",
			(int64_t*)TheJIT->lookup("SP").get().getValue()));
	}

	// Data
	{
		ret.insert(std::make_pair(
			"~DATA",
			(int64_t**)TheJIT->lookup("Data").get().getValue()));
		ret.insert(std::make_pair(
			"~DATA-PTR",
			(int64_t*)TheJIT->lookup("DP").get().getValue()));
	}

	for (auto& v: vars)
	{
		auto v1 = (uint64_t*)TheJIT->lookup(v).get().getValue();
		auto v2 = (int64_t*)*v1;
		ret.insert(std::make_pair(v, v2));
	}

	return ret;
}
