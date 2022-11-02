#include <capstone/capstone.h>
#include "CompilerLLVM.h"

void CompilerLLVM::SetupCapstone()
{
	// Setup capstone
	cs_opt_mem setup;
	setup.malloc = malloc;
	setup.calloc = calloc;
	setup.realloc = realloc;
	setup.free = free;
	setup.vsnprintf = vsnprintf;
	cs_err err = cs_option(0, CS_OPT_MEM, (size_t)&setup);
	if (err != CS_ERR_OK)
	{
		printf("Error (cs_option): %d\n", err);
	}
}

void CompilerLLVM::Disassemble(void* exec, size_t sz, size_t address)
{
	// Disassemble
	csh handle;
	cs_insn* insn;
	size_t count;
#ifdef CAPSTONE_HAS_X86
	printf("Disassembly architecture: X86\n");
	cs_err err = cs_open(CS_ARCH_X86, CS_MODE_64, &handle);
#else
	printf("Disassembly architecture: AARCH64\n");
	cs_err err = cs_open(CS_ARCH_ARM64, CS_MODE_ARM, &handle);
#endif
	if (err != CS_ERR_OK)
	{
		printf("Disassemble error: %d\n", err);
		exit(1);
	}
	cs_option(handle, CS_OPT_DETAIL, CS_OPT_ON);
	count = cs_disasm(handle, (const unsigned char*)exec, sz, address, 0, &insn);
	printf("There are %u CPU instructions\n", (unsigned)count);
	if (count > 0)
	{
		for (size_t j = 0; j < count; j++)
		{
			printf("0x%" PRIx64 ":\t%s\t%s\n", insn[j].address, insn[j].mnemonic, insn[j].op_str);
		}
		cs_free(insn, count);
	}
	else
	{
		printf("ERROR: Failed to disassemble given code!");
	}
	cs_close(&handle);
}
