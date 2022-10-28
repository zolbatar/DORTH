#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "compiler.h"
#include "stack.h"

// 29 instructions, 5 stack ops

#ifdef PITUBE
#include "../tube/swis.h"
#endif

extern void native_init();
cmap_str words;
clist_token tokens;
uint8_t base = 10;
jit_state_t* _jit;

void process_word(const char* word);

void compiler_init()
{
	native_init();
	setup_capstone();
	init_jit("Daric");
}

static void dump_ir()
{
	c_foreach (t, clist_token, tokens)
	{
		switch (t.ref->type)
		{
			case TOKEN_WORD:
				printf("WORD: '%s'\n", t.ref->word);
				break;
			case TOKEN_PUSH_INTEGER:
				printf("INTEGER: %d:%d\n", t.ref->v_i, t.ref->sequence);
				break;
			case TOKEN_PUSH_FLOAT:
				printf("FLOAT: %f:%d\n", t.ref->v_f, t.ref->sequence);
				break;
			case TOKEN_INC_SP:
				printf("INC SP: %d\n", t.ref->sequence);
				break;
			case TOKEN_DEC_SP:
				printf("DEC SP: %d\n", t.ref->sequence);
				break;
			case TOKEN_POP_INT0:
				printf("POP R0\n");
				break;
			case TOKEN_POP_INT1:
				printf("POP R1\n");
				break;
			case TOKEN_PUSH_INT0:
				printf("PUSH R0\n");
				break;
			case TOKEN_CALLNATIVE:
				printf("CALL %p\n", t.ref->native);
				break;

			case TOKEN_ADD:
				printf("R0=R0+R1\n");
				break;
		}
	}
}

static void optimise()
{
	clist_token_value* prev = NULL;
	c_foreach (t, clist_token, tokens)
	{
		if (prev != NULL && prev->type == t.ref->type)
		{
			t.ref->reset = true;
		}
		else
		{
			t.ref->reset = false;
		}
		switch (t.ref->type)
		{
			case TOKEN_WORD:
			{
				cmap_str_iter iter = cmap_str_find(&words, t.ref->word);
				if (iter.ref == NULL)
				{
					printf("Word '%s' not found\n", t.ref->word);
					return;
				}
				iter.ref->second.compile(&t);
				break;
			}
			case TOKEN_PUSH_INTEGER:
			case TOKEN_PUSH_FLOAT:
			case TOKEN_INC_SP:
			case TOKEN_DEC_SP:
			case TOKEN_PUSH_INT0:
			case TOKEN_POP_INT0:
			case TOKEN_POP_INT1:
				break;

			case TOKEN_CALLNATIVE:
			case TOKEN_ADD:
				break;
		}
		prev = t.ref;
	}
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
		process_word((const char*)&copy[start]);
	}

	optimise();
	dump_ir();

	// Now let's compile it
	_jit = jit_new_state();

	// This is the stub to call into the implicit function
	jit_prolog();
	stack_init();

	// Compile, we should have optimised by now
	c_foreach (t, clist_token, tokens)
	{
		switch (t.ref->type)
		{
			case TOKEN_WORD:
			{
				cmap_str_iter iter = cmap_str_find(&words, t.ref->word);
//				iter.ref->second.compile();
				break;
			}
			case TOKEN_PUSH_INTEGER:
				jit_movi(JIT_R0, t.ref->v_i);
				jit_stxi(t.ref->sequence * SS, JIT_V0, JIT_R0);
				break;
			case TOKEN_PUSH_FLOAT:
				jit_movi_d(JIT_F0, t.ref->v_f);
				jit_stxi_d(t.ref->sequence * SS, JIT_V0, JIT_F0);
				break;
			case TOKEN_INC_SP:
				jit_addi(JIT_V0, JIT_V0, t.ref->sequence * SS);
				break;
			case TOKEN_DEC_SP:
				jit_subi(JIT_V0, JIT_V0, t.ref->sequence * SS);
				break;
			case TOKEN_PUSH_INT0:
				jit_stxi(t.ref->sequence * SS, JIT_V0, JIT_R0);
				break;
			case TOKEN_POP_INT0:
				jit_ldxi(JIT_R0, JIT_V0, t.ref->sequence * SS);
				break;
			case TOKEN_POP_INT1:
				jit_ldxi(JIT_R1, JIT_V0, t.ref->sequence * SS);
				break;

			case TOKEN_CALLNATIVE:
				jit_prepare();
				jit_pushargr(JIT_R0);
				jit_finishi(t.ref->native);
				break;
			case TOKEN_ADD:
				jit_addr(JIT_R0, JIT_R0, JIT_R1);
				break;
		}
	}

	save_stack_ptr();
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