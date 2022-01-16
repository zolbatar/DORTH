#include <iostream>
#include "JIT.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/ExecutionEngine/Orc/LLJIT.h"
#include "llvm/Transforms/Scalar.h"
#include "WOFAlloc.h"
#include "../ErrorCodes.h"

wof_allocator_t* allocator;

void JIT::run() {
	auto jit = llvm::orc::LLJITBuilder().create();
	if (!jit) {
		std::cout << "Can't create JIT\n";
		exit(ERROR_CANT_CREATE_JIT);
	}
	auto JIT = jit->get();

	// Add compiled module (with linked in library)
	auto error = JIT->addIRModule(llvm::orc::ThreadSafeModule(std::move(module), std::move(context)));
	if (error) {
		std::cout << "Error adding IR module\n";
		exit(ERROR_ADDING_IR);
	}

	// Execute
	auto dl = JIT->getDataLayout();
	auto generator = JIT->getMainJITDylib()
			.addGenerator(std::move(
					llvm::orc::DynamicLibrarySearchGenerator::GetForCurrentProcess(dl.getGlobalPrefix()).get()));

	// Try and find the Implicit function, if fails then compilation likely failed
	auto proc_start = JIT->lookup("Implicit");
	auto entry = llvm::jitTargetAddressToFunction<void (*)
														  ()>(proc_start.get().getAddress());
	allocator = wof_allocator_new();
	entry();
	wof_allocator_destroy(allocator);
}