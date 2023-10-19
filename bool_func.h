#ifndef _BOOL_FUNC_H_
#define _BOOL_FUNC_H_

#include "expression.h"

typedef struct
{
	char vars[5];
	uint32_t min_terms;
} func_t;

func_t create_bool_func(expr_t *e);

#endif
