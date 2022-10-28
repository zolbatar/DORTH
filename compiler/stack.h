#ifndef _STACK_H_
#define _STACK_H_

void stack_init();
void save_stack_ptr();
void stack_push_int(int reg);
void stack_pop_int(int reg);
void stack_push_float(int reg);
void stack_pop_float(int reg);

#endif //_STACK_H_
