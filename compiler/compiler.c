#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "compiler.h"
#include "stack.h"
#ifdef PITUBE
#include "../tube/swis.h"
extern char get_key();
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

static void dump_nativeword(clist_token_iter* t, const char* desc)
{
	printf("%d/%d/%d [%s]\n", t->ref->pushes, t->ref->pops, t->ref->sequence, desc);
}

static void dump_ir()
{
	c_foreach (t, clist_token, tokens)
	{
		switch (t.ref->type)
		{
			case TOKEN_WORD:
				printf("%d/%d/%d [WORD   ]: '%s'\n", t.ref->pushes, t.ref->pops, t.ref->sequence, t.ref->word);
				break;
			case TOKEN_PUSH_INTEGER:
				printf("%d/%d/%d [INTEGER]: %d\n", t.ref->pushes, t.ref->pops, t.ref->sequence, t.ref->v_i);
				break;
			case TOKEN_PUSH_FLOAT:
				printf("%d/%d/%d [FLOAT  ]: %f\n", t.ref->pushes, t.ref->pops, t.ref->sequence, t.ref->v_f);
				break;
			case TOKEN_POP_R0:
				printf("%d/%d/%d [POP R0 ]\n", t.ref->pushes, t.ref->pops, t.ref->sequence);
				break;
			case TOKEN_POP_R1:
				printf("%d/%d/%d [POP R1 ]\n", t.ref->pushes, t.ref->pops, t.ref->sequence);
				break;
			case TOKEN_PUSH_R0:
				printf("%d/%d/%d [PUSH R0]\n", t.ref->pushes, t.ref->pops, t.ref->sequence);
				break;
			case TOKEN_INTEGER_TO_R0:
				printf("%d/%d/%d [SET R0 ]: %d\n", t.ref->pushes, t.ref->pops, t.ref->sequence, t.ref->v_i);
				break;
			case TOKEN_INTEGER_TO_R1:
				printf("%d/%d/%d [SET R1 ]: %d\n", t.ref->pushes, t.ref->pops, t.ref->sequence, t.ref->v_i);
				break;
			case TOKEN_CALLNATIVE:
				printf("%d/%d/%d [CALL   ]: %p\n", t.ref->pushes, t.ref->pops, t.ref->sequence, t.ref->native);
				break;

			case TOKEN_ADD:
				dump_nativeword(&t, "+      ");
				break;
			case TOKEN_SUBTRACT:
				dump_nativeword(&t, "-      ");
				break;
		}
	}
}

void test()
{
	printf("11111\n");
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

	expand();
	optimise();
	dump_ir();

	// Now let's compile it
	_jit = jit_new_state();
	jit_prolog();
	stack_init();

	// Compile, we should have optimised by now
	c_foreach (t, clist_token, tokens)
	{
		// Decrement stack?
		if (t.ref->pops > 0)
			jit_subi(JIT_V0, JIT_V0, t.ref->pops * SS);

		switch (t.ref->type)
		{
			case TOKEN_WORD:
				assert(0);
			case TOKEN_PUSH_INTEGER:
				jit_movi(JIT_R0, t.ref->v_i);
				jit_stxi(t.ref->sequence * SS, JIT_V0, JIT_R0);
				break;
			case TOKEN_PUSH_FLOAT:
				jit_movi_d(JIT_F0, t.ref->v_f);
				jit_stxi_d(t.ref->sequence * SS, JIT_V0, JIT_F0);
				break;
			case TOKEN_PUSH_R0:
				jit_stxi(t.ref->sequence * SS, JIT_V0, JIT_R0);
				break;
			case TOKEN_POP_R0:
				jit_ldxi(JIT_R0, JIT_V0, t.ref->sequence * SS);
				break;
			case TOKEN_POP_R1:
				jit_ldxi(JIT_R1, JIT_V0, t.ref->sequence * SS);
				break;
			case TOKEN_INTEGER_TO_R0:
				jit_movi(JIT_R0, t.ref->v_i);
				break;
			case TOKEN_INTEGER_TO_R1:
				jit_movi(JIT_R1, t.ref->v_i);
				break;

			case TOKEN_CALLNATIVE:
				jit_prepare();
				jit_pushargr(JIT_R0);
				jit_finishi(t.ref->native);
				break;
			case TOKEN_ADD:
				jit_addr(JIT_R0, JIT_R0, JIT_R1);
				break;
			case TOKEN_SUBTRACT:
				jit_subr(JIT_R0, JIT_R0, JIT_R1);
				break;
		}

		// Increment stack?
		if (t.ref->pushes > 0)
			jit_addi(JIT_V0, JIT_V0, t.ref->pushes * SS);

	}
	jit_ret();
	jit_epilog();

	// Code & data size
	jit_realize();
	if (!_jitc->realize)
	{
		printf("Failed to realise");
		return;
	}

//	jit_print();

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

//	disassemble(exec, code_size);

#ifdef PITUBE
	_swi(OS_SynchroniseCodeAreas, _IN(0), 0);
#endif

	jit_clear_state();
	printf("Preparing to execute at %p\n", exec);
	exec();
	printf("Execution complete\n");
	jit_destroy_state();
#ifndef RICH
	free(code);
#endif
	finish_jit();
	return;
}