#ifndef _COMPILER_H_
#define _COMPILER_H_

#include <stdbool.h>
#include "../lightning/lightning.h"
#include "../lightning/jit_private.h"
#include <capstone/capstone.h>

typedef int (* start)(void);

#ifdef __cplusplus
extern "C" {
#endif

void compiler_init();
void compile(const char* source);
void setup_capstone();
void disassemble(start exec, jit_word_t sz);

#ifdef __cplusplus
}
#endif

typedef enum
{
	TOKEN_PUSH_INTEGER,
	TOKEN_PUSH_FLOAT,
	TOKEN_INC_SP,
	TOKEN_DEC_SP,
	TOKEN_POP_INT0,
	TOKEN_POP_INT1,
	TOKEN_PUSH_INT0,
	TOKEN_WORD,
	TOKEN_CALLNATIVE,

	TOKEN_ADD,
} token_type;

typedef struct
{
	token_type type;
	unsigned sequence;
	bool reset;
	union
	{
		const char* word;
		void* native;
		int v_i;
		double v_f;
	};
} token;

static int token_cmp(const token* a, const token* b)
{
	return a == b;
}
#define i_val token
#define i_cmp token_cmp
#include "../stc/clist.h"

typedef struct
{
	void (* compile)(clist_token_iter* t);
	void (* interpret)(void);
} word;

#include "../stc/cstr.h"
#define i_key_str
#define i_val word
#include "../stc/cmap.h"

#endif //_COMPILER_H_
