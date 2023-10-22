#include <stdio.h>
#include <string.h>

#include "expression.h"
#include "function.h"

// const char test[] = "(x' . Y + x . Y')' . Z + (X' . Y + X . Y') . Z'";
const char test[] = "A . B + A . C + A . B";

int main(void)
{
	expr_t *e = parse_expr(test, sizeof test - 1);
	debug_print_expr(e, 0);

	env_t huh = {0};
	memset(huh.vars, -1, sizeof huh.vars);

	func_t f = create_func(e, huh);
	debug_print_func(&f);

	/*
	term_t *t = generate_singleton_terms(&f);
	debug_print_terms(t);
	*/

	delete_expr(e);
	return 0;
}
