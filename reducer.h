#ifndef _REDUCER_H_
#define _REDUCER_H_

#include "expression.h"
#include "function.h"

typedef struct
{
	uint32_t actives;
	uint32_t values;
} term_t;

typedef struct
{
	term_t *terms;
	size_t count;
} term_list_t;

term_list_t terms_copy(func_t *f);

int reduce_term(term_list_t *a, term_t t, term_list_t *b);
term_list_t reduce_term_list(term_list_t *a, func_t *f);
expr_t *reducer_create_expr(term_list_t *a, func_t *f, int p);

void debug_print_term(term_t t, func_t *df);
void debug_print_term_list(term_list_t t, func_t *f);

#endif
