#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "bool_func.h"

func_t create_bool_func(expr_t *e, env_t *d)
{
	func_t f = {0};

	f.min_terms = calloc(1 << d->count, sizeof(char));
	f.count = d->count;

	for(int i = 0, c = 0; i < 26; i++)
	{
		if(d->vars[i] != -1)
		{
			f.vars[c] = i + 'A';
		}
	}

	for(int32_t i = 0; i < (1 << d->count); i++)
	{
		d->value = i;
		f.min_terms[i] = eval_expr(e, d);
	}

	return f;
}

/*
 * Create Multi Dimention map.
 * 	Identify the Single tons.
 * 	Select all the prime implicants.
 *	Mark off the implicants already chosen.
 */

static expr_t *SOP(int32_t t, char *vars, int count)
{
	char buffer[count * 3];
	memset(buffer, 0, sizeof buffer);

	int c = 0;
	for(int i = 0; i < count; i++)
	{
		if(((t >> i) & 1) == 1)
		{
			buffer[c++] = vars[i];
			buffer[c++] = '.';
		}
		else
		{
			buffer[c++] = vars[i];
			buffer[c++] = '\'';
			buffer[c++] = '.';
		}
	}

	printf("%.*s\n", c, buffer);

	return parse_expr(buffer, c - 1);
}

expr_t *reduce_bool_func(func_t f)
{
	for(int i = 0; i < f.count; i++)
	{
		printf("%c : %d\n", f.vars[i], i);
	}
	expr_t *e = SOP(0, f.vars, f.count);
	return e;
}
