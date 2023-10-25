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
	
	int tatcontra; // is tautology or contradiction
} func_t;

func_t create_func(expr_t *exp, env_t env);
void delete_function(func_t *f);
void debug_print_func(func_t *f);

expr_t *reduce_func(func_t *f);

#endif
