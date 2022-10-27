#include "stack.h"
#include "../lightning/lightning.h"
#include "../lightning/jit_private.h"

extern jit_state_t* _jit;
#define STACK_SIZE 2048

void stack_init()
{
	jit_movi(JIT_V0, jit_allocai(STACK_SIZE));
}

void stack_push_int(int reg)
{
	jit_stxr_i(JIT_V0, JIT_FP, reg);
	jit_addi(JIT_V0, JIT_V0, 4);
}

void stack_pop_int(int reg)
{
	jit_subi(JIT_V0, JIT_V0, 4);
	jit_ldxr_i(reg, JIT_FP, JIT_V0);
}

void stack_push_float(int reg)
{
	jit_stxr_d(JIT_V0, JIT_FP, reg);
	jit_addi(JIT_V0, JIT_V0, 8);
}

void stack_pop_float(int reg)
{
	jit_subi(JIT_V0, JIT_V0, 8);
	jit_ldxr_d(reg, JIT_FP, JIT_V0);
}

