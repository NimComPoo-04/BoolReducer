#include <stdio.h>
#include <string.h>

#include "expression.h"

// const char test[] = "A + B' * C + D";
//   +
//  + D
// A *
//  ' C
//  B

const char test[] = "A' . B + B' . A + A . B + A' . B'";

int main(void)
{
	/*
	env_t e = {0};

	expr_t a = {.type = VARIABLE, .literal = 'A'};
	expr_t b = {.type = LITERAL, .literal = '1'};
	expr_t c = {.type = OR, .or = {&a, &b}};
	expr_t d = {.type = AND, .or = {&a, &b}};
	expr_t k = {.type = OR, .or = {&c, &d}};

	debug_print_expr(&k, 0);

	printf("%d + %d = %d\n", eval_expr(&a, &e), eval_expr(&b, &e), eval_expr(&c, &e));
	printf("%d . %d = %d\n", eval_expr(&a, &e), eval_expr(&b, &e), eval_expr(&d, &e));
	*/

	/*
	size_t current = 0;
	char c = 1;
	while(c)
	{
		c = next_token(test, sizeof test - 1, &current);
		putchar(c);
	}
	*/

	env_t e = {0};
	memset(&e, -1, sizeof e);
	expr_t *a = parse_expr(test, sizeof test - 1);
	debug_print_expr(a, 0);
	printf("%s = %d\n", test, eval_expr(a, &e));
	delete_expr(a);

	return 0;
}
