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

void native_DOT(clist_token_iter* t)
{
	clist_token_insert_at(&tokens, *t, (token){ TOKEN_CALLNATIVE, .native = &__DOT });
	clist_token_insert_at(&tokens, *t, (token){ TOKEN_POP_INT0, 0 });
	clist_token_insert_at(&tokens, *t, (token){ TOKEN_DEC_SP, 1 });
}

void native_PLUS(clist_token_iter* t)
{
	clist_token_insert_at(&tokens, *t, (token){ TOKEN_INC_SP, 1 });
	clist_token_insert_at(&tokens, *t, (token){ TOKEN_PUSH_INT0, 0 });
	clist_token_insert_at(&tokens, *t, (token){ TOKEN_ADD });
	clist_token_insert_at(&tokens, *t, (token){ TOKEN_POP_INT1, 0 });
	clist_token_insert_at(&tokens, *t, (token){ TOKEN_DEC_SP, 1 });
	clist_token_insert_at(&tokens, *t, (token){ TOKEN_POP_INT0, 0 });
	clist_token_insert_at(&tokens, *t, (token){ TOKEN_DEC_SP, 1 });
}

void native_init()
{
	words = cmap_str_init();
	cmap_str_emplace(&words, ".", (word){ &native_DOT, NULL });
	cmap_str_emplace(&words, "+", (word){ &native_PLUS, NULL });
}