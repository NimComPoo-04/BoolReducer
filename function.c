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
	printf("\n == TRUTH TABLES == \n\n");

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
	debug_print_term_list(t);

	t = reduce_term_list(&t);

	// TODO: remove FOR DEBUG
	printf("\n== REDUCED ==");
	debug_print_term_list(t);

	char *buffer = calloc(sizeof(char), 1024);
	int len = 0;
	int size = 1024;

	char tmp[f->count * 3 + 1];
	memset(tmp, 0, sizeof tmp);

	int ct = 0;

	for(int i = 0; i < t.count; i++)
	{
		term_t x = t.terms[i];
		int k = 0;
		ct = 0;

		while(x.actives)
		{

			if(x.actives & 1)
			{
				tmp[ct++] = f->vars[k];
				if((x.values & 1) == 0)
					tmp[ct++] = '\'';
				tmp[ct++] = '.';
			}

			k++;
			x.actives >>= 1;
			x.values  >>= 1;
		}

		ct--;
		tmp[ct++] = '+';

		if(len >= size)
		{
			size += len;
			buffer = realloc(buffer, size);
		}

		strncat(buffer + len, tmp, ct);
		len += ct;
	}

	len--;
	buffer[len] = 0;

	printf("\n== EXPR: %s\n", buffer);

	expr_t *e = parse_expr(buffer, len);
	free(buffer);

	return e;
}
