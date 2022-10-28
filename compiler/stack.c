#include "stack.h"
#include "../lightning/lightning.h"
#include "../lightning/jit_private.h"

extern jit_state_t* _jit;
#define STACK_SIZE 2048

void stack_init()
{
	jit_movi(JIT_V0, jit_allocai(STACK_SIZE));
	jit_negr(JIT_V0, JIT_V0);
	jit_addr(JIT_V0, JIT_V0, JIT_FP);
}

void stack_push_int(int reg)
{
	jit_str(JIT_V0, reg);
	jit_addi(JIT_V0, JIT_V0, sizeof(size_t));
}

void stack_pop_int(int reg)
{
	jit_subi(JIT_V0, JIT_V0, sizeof(size_t));
	jit_ldr(reg, JIT_V0);
}

void stack_push_float(int reg)
{
	jit_str_d(JIT_V0, reg);
	jit_addi(JIT_V0, JIT_V0, sizeof(double));
}

void stack_pop_float(int reg)
{
	jit_subi(JIT_V0, JIT_V0, sizeof(double));
	jit_ldr_d(reg, JIT_V0);
}

