#ifndef _FUNCTION_H_
#define _FUNCTION_H_

#include "expression.h"

typedef struct
{
	env_t env;	 // copy of the environment
	int count;	// number of variables
	char vars[26]; // An array filled with the variables in the order of
		       // discovery, useful for printing? idk
	char *min_terms; // Caching the function
} func_t;

func_t create_func(expr_t *exp, env_t env);
void debug_print_func(func_t *f);

// Reduction Without using K-MAP

typedef struct term_t
{
	uint32_t values;
	uint32_t active_vars;

	struct term_t *next;
} term_t;

term_t *generate_singleton_terms(func_t *f);
void debug_print_terms(term_t *t);

// try to reduce terms, returns 1 if possible else return 0
int reduce_terms(func_t *f, term_t *t, int level);


#endif
