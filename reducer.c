#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <memory.h>

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

term_list_t reduce_term_list(term_list_t *a)
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
//				debug_print_term(a->terms[i]);
//				puts("");
			}
			else
				t++;
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
