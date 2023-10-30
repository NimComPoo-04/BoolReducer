#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "function.h"
#include "reducer.h"

func_t create_func(expr_t *exp, env_t env)
{
	func_t f = {0};

	f.env = env;

	eval_expr(exp, &env);
	f.min_terms = calloc(sizeof(char), 1 << env.count);

	int lt = 0;
	for(uint32_t i = 0; i < (uint32_t)(1 << env.count); i++)
	{
		env.value = i;
		f.min_terms[i] = eval_expr(exp, &env);
		if(f.min_terms[i])
			lt++;
	}

	if(lt == 0)
		f.tatcontra = 1;
	else if(lt == (1 << env.count))
		f.tatcontra = 2;

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
	printf("\n== TRUTH TABLES == \n\n");

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

void delete_function(func_t *f)
{
	free(f->min_terms);
}

expr_t *reduce_func(func_t *f)
{
	if(f->tatcontra == 1)
		return parse_expr("0", 1);
	else if(f->tatcontra == 2)
		return parse_expr("1", 1);

	term_list_t t = terms_copy(f);

	// TODO: remove FOR DEBUG
	printf("\n== UNREDUCED ==");
	debug_print_term_list(t, f);
	puts("");

	// FIXME: this is a haenous warcrime, change it. no need to heap allocate anything
	printf("\n== STEPS [ Idempotence, Distributive ] ==\n\n");
	expr_t *e = reducer_create_expr(&t, f, 0);
	printf("   ");
	debug_print_expr_oneline(e);
	puts("");
	delete_expr(e);
	t = reduce_term_list(&t, f);
	puts("");

	// TODO: remove FOR DEBUG
	printf("\n== REDUCED ==");
	debug_print_term_list(t, f);

	return reducer_create_expr(&t, f, 0);
}
