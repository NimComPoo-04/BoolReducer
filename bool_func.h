#ifndef _BOOL_FUNC_H_
#define _BOOL_FUNC_H_

#define MAX_VARS 26

#include "expression.h"

typedef struct
{
	int count;
	char vars[MAX_VARS];
	char *min_terms;
} func_t;

func_t create_bool_func(expr_t *e, env_t *d);
// returns a reduced expression
expr_t *reduce_bool_func(func_t f);

#endif
