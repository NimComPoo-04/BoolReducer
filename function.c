#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "function.h"

func_t create_func(expr_t *exp, env_t env)
{
	func_t f = {0};

	f.env = env;

	eval_expr(exp, &env);
	f.min_terms = calloc(sizeof(char), 1 << env.count);

	for(uint32_t i = 0; i < (uint32_t)(1 << env.count); i++)
	{
		env.value = i;
		f.min_terms[i] = eval_expr(exp, &env);
	}

	f.count = env.count;
	for(int i = 0; i < 26; i++)
	{
		if(env.vars[i] != -1)
			f.vars[(int)env.vars[i]] = i + 'A';
	}

	return f;
}

void debug_print_func(func_t *f)
{
	printf(" == Truth Table == \n\n");

	for(int i = 0; i < f->count; i++)
		printf(" %c |", f->vars[i]);
	printf(" OUT \n");

	for(int32_t i = 0; i < (1 << f->count); i++)
	{
		for(int32_t j = 0; j < f->count; j++)
		{
			printf(" %d |", (i >> j) & 1);
		}
		printf("  %d\n", f->min_terms[i]);
	}
}

term_t *generate_singleton_terms(func_t *f)
{
	term_t *head = NULL;

	for(uint32_t i = 0; i < (1 << f->count); i++)
	{
		if(f->min_terms[i])
		{
			term_t *h = malloc(sizeof(term_t));
			h->values = i;
			h->active_vars = (1 << f->count) - 1;
			h->next = NULL;

			if(head == NULL)
				head = h;
			else
			{
				h->next = head;
				head = h;
			}
		}
	}

	return head;
}

void debug_print_terms(term_t *t)
{
	if(!t) return;
	printf("%d : %d\n", t->active_vars, t->values);
	puts("");
	debug_print_terms(t->next);
}

int reduce_terms(func_t *f, term_t *t, int level)
{
	if(!t) return 0;
}
