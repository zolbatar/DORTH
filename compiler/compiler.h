#ifndef _COMPILER_H_
#define _COMPILER_H_

#ifdef __cplusplus
extern "C" {
#endif

void compile(const char* source);

#ifdef __cplusplus
}
#endif

typedef enum
{
	TOKEN_NONE,
	TOKEN_WORD,
	TOKEN_INTEGER,
	TOKEN_FLOAT
} token_type;

typedef struct
{
	token_type type;
} token;

#endif //_COMPILER_H_
