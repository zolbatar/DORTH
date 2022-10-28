#include "compiler.h"

extern jit_state_t* _jit;

void setup_capstone()
{
#ifndef DISABLE_DISASM
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
#endif
}

void disassemble(start exec, jit_word_t sz)
{
#ifndef DISABLE_DISASM
	// Disassemble
	csh handle;
	cs_insn* insn;
	size_t count;
#ifdef CAPSTONE_HAS_X86
	printf("Disassembly architecture: X86\n");
	cs_err err = cs_open(CS_ARCH_X86, CS_MODE_64, &handle);
#else
#ifdef PITUBE
	printf("Disassembly architecture: ARM\n");
	cs_err err = cs_open(CS_ARCH_ARM, CS_MODE_ARM, &handle);
#else
	printf("Disassembly architecture: AARCH64\n");
	cs_err err = cs_open(CS_ARCH_ARM64, CS_MODE_ARM, &handle);
#endif
#endif
	if (err != CS_ERR_OK)
	{
		printf("Disassemble error: %d\n", err);
	}
	cs_option(handle, CS_OPT_DETAIL, CS_OPT_ON);
	count = cs_disasm(handle, (const unsigned char*)exec, sz, (size_t)_jit->code.ptr, 0, &insn);
	printf("There are %u CPU instructions\n", (unsigned)count);
	if (count > 0)
	{
		for (size_t j = 0; j < count; j++)
		{
#ifdef PITUBE
			printf("0x%X:\t%s\t%s\n", (uint32_t)insn[j].address, insn[j].mnemonic, insn[j].op_str);
#else
			printf("0x%" PRIx64 ":\t%s\t%s\n", insn[j].address, insn[j].mnemonic, insn[j].op_str);
#endif
		}
		cs_free(insn, count);
	}
	else
	{
		printf("ERROR: Failed to disassemble given code!");
	}
	cs_close(&handle);
#endif
}

