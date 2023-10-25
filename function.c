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
	printf("\n == Truth Table == \n\n");

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

/*
expr_t *reduce_func(func_t *f)
{
	(void)f;
	return NULL;
}
*/

void delete_function(func_t *f)
{
	free(f->min_terms);
}
