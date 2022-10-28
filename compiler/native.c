#include "compiler.h"
#include "stack.h"
#include "../lightning/lightning.h"
#include "../lightning/jit_private.h"

#ifndef PITUBE
void console_print(const char*);
#endif

extern clist_token tokens;
extern cmap_str words;
extern jit_state_t* _jit;

void __DOT(int i)
{
#ifdef PITUBE
	printf("%d ", i);
#else
	char buffer[2048];
	sprintf(buffer, "%d ", i);
	console_print(buffer);
#endif
}

void native_DOT(clist_token_iter* t)
{
	*t = clist_token_insert_at(&tokens, *t, (token){ TOKEN_CALLNATIVE, 0, 0, 0, .native = &__DOT });
	*t = clist_token_insert_at(&tokens, *t, (token){ TOKEN_POP_R0, 0, 1, 0 });
}

void native_MINUS(clist_token_iter* t)
{
	*t = clist_token_insert_at(&tokens, *t, (token){ TOKEN_PUSH_R0, 0, 0, 1 });
	*t = clist_token_insert_at(&tokens, *t, (token){ TOKEN_SUBTRACT });
	*t = clist_token_insert_at(&tokens, *t, (token){ TOKEN_POP_R0, 0, 1, 0 });
	*t = clist_token_insert_at(&tokens, *t, (token){ TOKEN_POP_R1, 0, 1, 0 });
}

void native_PLUS(clist_token_iter* t)
{
	*t = clist_token_insert_at(&tokens, *t, (token){ TOKEN_PUSH_R0, 0, 0, 1 });
	*t = clist_token_insert_at(&tokens, *t, (token){ TOKEN_ADD });
	*t = clist_token_insert_at(&tokens, *t, (token){ TOKEN_POP_R0, 0, 1, 0 });
	*t = clist_token_insert_at(&tokens, *t, (token){ TOKEN_POP_R1, 0, 1, 0 });
}

void native_DROP(clist_token_iter* t)
{
	*t = clist_token_insert_at(&tokens, *t, (token){ TOKEN_POP_R0, 0, 1, 0 });
}

void native_init()
{
	words = cmap_str_init();
	cmap_str_emplace(&words, ".", (word){ &native_DOT, NULL });
	cmap_str_emplace(&words, "-", (word){ &native_MINUS, NULL });
	cmap_str_emplace(&words, "+", (word){ &native_PLUS, NULL });
	cmap_str_emplace(&words, "DROP", (word){ &native_DROP, NULL });
}