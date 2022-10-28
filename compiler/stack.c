#include "stack.h"
#include "../lightning/lightning.h"
#include "../lightning/jit_private.h"

extern jit_state_t* _jit;
#define STACK_SIZE 2048
size_t stack[STACK_SIZE];
size_t stack_ptr;

void stack_init()
{
	jit_movi(JIT_V0, (jit_word_t)&stack);
}

void save_stack_ptr()
{
	jit_sti((jit_pointer_t)&stack_ptr, JIT_V0);
}

void stack_push_int(int reg)
{
	jit_str(JIT_V0, reg);
	jit_addi(JIT_V0, JIT_V0, SS);
}

void stack_pop_int(int reg)
{
	jit_subi(JIT_V0, JIT_V0, SS);
	jit_ldr(reg, JIT_V0);
}

void stack_push_float(int reg)
{
	jit_str_d(JIT_V0, reg);
	jit_addi(JIT_V0, JIT_V0, SS);
}

void stack_pop_float(int reg)
{
	jit_subi(JIT_V0, JIT_V0, SS);
	jit_ldr_d(reg, JIT_V0);
}

