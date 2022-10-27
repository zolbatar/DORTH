#ifndef _COMPILER_H_
#define _COMPILER_H_

#ifdef __cplusplus
extern "C" {
#endif

void compiler_init();
void compile(const char* source);

#ifdef __cplusplus
}
#endif

typedef enum
{
	TOKEN_WORD,
	TOKEN_INTEGER,
	TOKEN_FLOAT
} token_type;

typedef struct
{
	token_type type;
	union
	{
		const char* word;
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

#endif //_COMPILER_H_
