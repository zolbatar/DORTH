#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "compiler.h"
#include "../lightning/lightning.h"
#include "../lightning/jit_private.h"
#include <capstone/capstone.h>

clist_token tokens;
uint8_t base = 10;
jit_state_t* _jit;
typedef int (* start)(void);

void process_word(const char* word);

static void setup_capstone()
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

static void disassemble(start exec, jit_word_t sz)
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
	printf("There are %u CPU instructions\n", count);
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

void compiler_init()
{
	setup_capstone();
	init_jit("Daric");
}

void compile(const char* source)
{
	tokens = clist_token_init();
	size_t l = strlen(source);
	if (l == 0)
		return;

	// Copy it
	char copy[l];
	strncpy(copy, source, l);
	copy[l] = 0;

	// Process one token at a time
	int start = -1;
	int last_end = 0;
	int end = 0;
	for (int i = 0; i < l; i++)
	{
		char c = source[i];
		if (isblank(c))
		{
			// Previous word?
			if (start != -1)
			{
				end = i;
				copy[end] = 0;
				process_word((const char*)&copy[start]);
				last_end = end;
				start = -1;
			}
			else
			{
				// Absorb
			}
		}
		else
		{
			if (start == -1)
				start = i;
		}
	}

	// Did we process the last word?
	if (last_end != l && start != -1)
	{
		end = l;
		copy[end] = 0;
		process_word((const char*)&copy[start]);
	}

	// Dump out list of tokens
	c_foreach (t, clist_token, tokens)
	{
		switch (t.ref->type)
		{
			case TOKEN_WORD:
				printf("WORD: '%s'\n", t.ref->word);
				break;
			case TOKEN_INTEGER:
				printf("INTEGER: %d\n", t.ref->v_i);
				break;
			case TOKEN_FLOAT:
				printf("FLOAT: %f\n", t.ref->v_f);
				break;
		}
	}

	// Now let's compile it
	_jit = jit_new_state();

	// This is the stub to call into the implicit function
	jit_prolog();
	jit_ret();
	jit_epilog();

	// Code & data size
	jit_realize();
	if (!_jitc->realize)
	{
		printf("Failed to realise");
		return;
	}

	jit_print();

	// Do compile
	jit_word_t sz = _jit->code.length;
#ifndef RICH
	void* code = malloc(sz);

	jit_set_code(code, sz);
	jit_set_data(NULL, 0, JIT_DISABLE_NOTE | JIT_DISABLE_NOTE);
#endif
	int (* exec)(void) = jit_emit_void();
	if (exec == NULL)
	{
		printf("Code generation failed");
		return;
	}

	// Size?
	jit_word_t code_size;
	jit_get_code(&code_size);
#ifdef VERBOSE_COMPILE
	printf("Code size: %ld [%ld] bytes\n", code_size, sz);
#endif

#ifdef PITUBE
	_swi(OS_SynchroniseCodeAreas, _IN(0), 0);
#endif

	disassemble(exec, code_size);

	jit_clear_state();
	printf("Preparing to execute\n");
	exec();
	printf("Execution complete\n");
	jit_destroy_state();
#ifndef RICH
	free(code);
#endif
	finish_jit();
	return;
}

void process_word(const char* word)
{
	size_t l = strlen(word);
	char* end;

	// Decimal
	if (word[0] == '#')
	{
		long i = strtol(&word[1], &end, 10);
		if (end != word)
		{
			token t;
			t.type = TOKEN_INTEGER;
			t.v_i = i;
			clist_token_push_back(&tokens, t);
			return;
		}
		else
		{
			printf("Error parsing integer literal\n");
		}
	}

	// Hexadecimal
	if (word[0] == '$')
	{
		long i = strtol(&word[1], &end, 16);
		if (end != word)
		{
			token t;
			t.type = TOKEN_INTEGER;
			t.v_i = i;
			clist_token_push_back(&tokens, t);
			return;
		}
		else
		{
			printf("Error parsing integer literal\n");
		}
	}

	// Binary
	if (word[0] == '%')
	{
		long i = strtol(&word[1], &end, 2);
		if (end != word)
		{
			token t;
			t.type = TOKEN_INTEGER;
			t.v_i = i;
			clist_token_push_back(&tokens, t);
			return;
		}
		else
		{
			printf("Error parsing integer literal\n");
		}
	}

	// Float?
	if (strstr(word, "."))
	{
		double d = strtod(word, &end);
		if (end != word)
		{
			token t;
			t.type = TOKEN_FLOAT;
			t.v_f = d;
			clist_token_push_back(&tokens, t);
			return;
		}
	}

	// Base?
	long i = strtol(word, &end, base);
	if (end != word)
	{
		token t;
		t.type = TOKEN_INTEGER;
		t.v_i = i;
		clist_token_push_back(&tokens, t);
		return;
	}

	// Gotta be a word!
	token t;
	t.type = TOKEN_WORD;
	t.word = word;
	clist_token_push_back(&tokens, t);
	return;
}