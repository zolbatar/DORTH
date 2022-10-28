#include "compiler.h"

extern clist_token tokens;
cmap_str words;

#define COMPACT_STACKS

void expand()
{
	clist_token_value* prev = NULL;
	for (clist_token_iter t = clist_token_begin(&tokens); t.ref; /*clist_token_next(&t)*/)
	{
		switch (t.ref->type)
		{
			case TOKEN_WORD:
			{
				cmap_str_iter iter = cmap_str_find(&words, t.ref->word);
				if (iter.ref == NULL)
				{
					printf("Word '%s' not found\n", t.ref->word);
#ifndef PITUBE
					exit(1);
#endif
					return;
				}
				t = clist_token_erase_at(&tokens, t);
				iter.ref->second.compile(&t);
				break;
			}
			default:
				clist_token_next(&t);
				break;
		}
	}
}

/*
 * Phase 1 - Integer push followed by integer pop
 */
void optimise_phase1()
{
	for (clist_token_iter t = clist_token_begin(&tokens); t.ref;)
	{
		clist_token_iter t2 = clist_token_advance(t, 1);
		clist_token_iter t3 = clist_token_advance(t, 2);
		clist_token_iter t4 = clist_token_advance(t, 3);

		// Go in order of revere complexity so we do the most complex first
		if (t4.ref != NULL)
		{
			if (t.ref->type == TOKEN_PUSH_INTEGER
				&& t2.ref->type == TOKEN_PUSH_INTEGER
				&& t3.ref->type == TOKEN_POP_R1
				&& t4.ref->type == TOKEN_POP_R0)
			{
				t.ref->type = TOKEN_INTEGER_TO_R0;
				t.ref->pushes--;
				t2.ref->type = TOKEN_INTEGER_TO_R1;
				t2.ref->pushes--;

				clist_token_erase_at(&tokens, t4);
				clist_token_erase_at(&tokens, t3);
				clist_token_next(&t);
				goto cont;
			}

		}
		if (t2.ref != NULL)
		{
			if (t.ref->type == TOKEN_PUSH_INTEGER && t2.ref->type == TOKEN_POP_R0)
			{
				t.ref->type = TOKEN_INTEGER_TO_R0;
				t.ref->pushes--;

				clist_token_erase_at(&tokens, t2);
				clist_token_next(&t);
				goto cont;
			}
			if (t.ref->type == TOKEN_PUSH_R0 && t2.ref->type == TOKEN_POP_R0)
			{
				// Delete them both as counteract
				clist_token_erase_at(&tokens, t2);
				clist_token_erase_at(&tokens, t);
				t = t3;
				goto cont;
			}
		}

		clist_token_next(&t);
	cont:
		{};
	}
}

void optimise_phase2()
{
	clist_token_value* prev = NULL;
	c_foreach (t, clist_token, tokens)
	{
		// Optimise stack ops
#ifdef COMPACT_STACKS
		if (prev != NULL && prev->type == t.ref->type)
		{
			t.ref->pushes = prev->pushes + 1;
			prev->pushes = 0;
			t.ref->sequence = prev->sequence + 1;
		}
#endif
		switch (t.ref->type)
		{
			case TOKEN_WORD:
				assert(0);
			case TOKEN_PUSH_INTEGER:
			case TOKEN_PUSH_FLOAT:
			case TOKEN_PUSH_R0:
				break;
			case TOKEN_POP_R0:
#ifdef COMPACT_STACKS
				if (prev->type == TOKEN_POP_R1)
				{
					prev->pops = t.ref->pops + 1;
					t.ref->pops = 0;
					t.ref->sequence = prev->sequence + 1;
				}
#endif
				break;
			case TOKEN_POP_R1:
			case TOKEN_INTEGER_TO_R0:
			case TOKEN_INTEGER_TO_R1:
				break;

			case TOKEN_CALLNATIVE:
			case TOKEN_ADD:
			case TOKEN_SUBTRACT:
				break;
		}
		prev = t.ref;
	}
}

void optimise()
{
	optimise_phase1();
	optimise_phase2();
}
