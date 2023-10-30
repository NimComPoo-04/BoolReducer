#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <memory.h>
#include <string.h>
#include <assert.h>

#include "reducer.h"

term_list_t terms_copy(func_t *f)
{
	term_list_t t = {0};
	t.terms = calloc(sizeof(term_t), 1 << f->count);
	t.count = 0;

	uint32_t active = (1 << f->count) - 1;

	for(uint32_t i = 0; i < (unsigned)(1 << f->count); i++)
	{
		if(f->min_terms[i] == 1)
		{
			t.terms[t.count].actives = active;
			t.terms[t.count++].values = i;
		}
	}

	return t;
}

static int bits(uint32_t a)
{
	if(a == 0) return 0;

	double k = log2(a);
	return (uint32_t)((k - floor(k)) * 10E6) == 0;
}

static term_t deactivate(term_t a, term_t b)
{
	term_t t = {0};

	uint32_t va = (a.values ^ b.values);
	uint32_t k = (uint32_t)log2(va);

	t.actives = a.actives & ~(1 << k);
	t.values = a.values & ~(1 << k);

	return t;
}

static size_t term_list_size = 0;
int set_add(term_list_t *t, term_t g)
{
	for(unsigned i = 0; i < t->count; i++)
	{
		if(t->terms[i].values == g.values && t->terms[i].actives == g.actives)
			return 0;
	}

	if(t->count >= term_list_size)
	{
		term_list_size += t->count;
		t->terms = realloc(t->terms, sizeof(term_t) * term_list_size);
	}
	t->terms[t->count++] = g;
	return 1;
}

int reduce_term(term_list_t *a, term_t t, term_list_t *b)
{
	int bi = 0;
	for(size_t i = 0; i < a->count; i++)
	{
		if(a->terms[i].actives == t.actives)
		{
			if(bits(a->terms[i].values ^ t.values))
			{
				term_t f = deactivate(a->terms[i], t);
				set_add(b, f);
				bi = 1;
			}
		}
	}
	return bi;
}

term_list_t reduce_term_list(term_list_t *a, func_t *f)
{
	term_list_t x = {0};
	x.terms = calloc(sizeof(term_t), a->count);

	int t = 0;
	do
	{
		t = 0;
		term_list_t b = {0};
		b.terms = calloc(sizeof(term_t), a->count);
		term_list_size = a->count;

		for(unsigned i = 0; i < a->count; i++)
		{
			if(!reduce_term(a, a->terms[i], &b))
			{
				x.terms[x.count++] = a->terms[i];
//				debug_print_term(a->terms[i], f);
//				puts("");
			}
			else
				t++;
		}

		if(b.count > 0)
		{
			printf("=> ");  

			expr_t *tmp = reducer_create_expr(&b, f, 0);
			debug_print_expr_oneline(tmp);
			delete_expr(tmp);

			if(x.count > 0)
			{
				printf(" + ");
				tmp = reducer_create_expr(&x, f, 0);
				debug_print_expr_oneline(tmp);
				delete_expr(tmp);
			}
			puts("");
		}

		free(a->terms);
		memcpy(a, &b, sizeof b);
	}
	while(t);

	free(a->terms);
	return x;
}

void debug_print_term(term_t k, func_t *df)
{
	int f = 0;

	for(int i = 0; i < df->count; i++)
	{
		if(k.actives & 1)
		{
			if((k.values & 1) == 0)
				printf("%c' ", df->vars[f]);
			else
				printf("%c  ", df->vars[f]);
		}
		else
			printf("-  ");

		k.actives >>= 1;
		k.values >>= 1;
		f++;
	}
}

void debug_print_term_list(term_list_t t, func_t *df)
{
	puts("\n== TERMS ==\n");
	for(unsigned i = 0; i < t.count; i++)
	{
		debug_print_term(t.terms[i], df);
		puts("");
	}
}

expr_t *reducer_create_expr(term_list_t *t, func_t *f, int print_it)
{
	char *buffer = calloc(sizeof(char), 1024);
	int len = 0;
	int size = 1024;

	char tmp[f->count * 3 + 1];
	memset(tmp, 0, sizeof tmp);

	int ct = 0;

	for(unsigned i = 0; i < t->count; i++)
	{
		term_t x = t->terms[i];
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

		if(len + ct >= size)
		{
			size += 1024;
			buffer = realloc(buffer, size);
		}

		strncat(buffer + len, tmp, ct);
		len += ct;
	}

	len--;
	buffer[len] = 0;

	if(print_it)
		printf("  %s", buffer);

	expr_t *e = parse_expr(buffer, len);
	free(buffer);

	return e;
}
