#include "compiler.h"
#include "stack.h"
#include "../lightning/lightning.h"
#include "../lightning/jit_private.h"

#ifndef PITUBE
void console_print(const char*);
#endif

extern cmap_str words;
extern jit_state_t* _jit;

static void __DOT(int i)
{
#ifdef PITUBE
	printf("%d ", i);
#else
	char buffer[2048];
	sprintf(buffer, "%d ", i);
	console_print(buffer);
#endif
}

void native_DOT()
{
	stack_pop_int(JIT_R0);
	jit_prepare();
	jit_pushargr(JIT_R0);
	jit_finishi(&__DOT);
}

void native_PLUS()
{
	stack_pop_int(JIT_R0);
	stack_pop_int(JIT_R1);
	jit_addr(JIT_R0, JIT_R1, JIT_R1);
	stack_push_int(JIT_R0);
}

void native_init()
{
	words = cmap_str_init();
	cmap_str_emplace(&words, ".", (word){ &native_DOT, NULL });
	cmap_str_emplace(&words, "+", (word){ &native_PLUS, NULL });
}