#include <stdio.h>
#include <string.h>

#include "expression.h"
#include "bool_func.h"

// const char test[] = "A + B' * C + D";
//   +
//  + D
// A *
//  ' C
//  B

const char test[] = "A . B' + A' . B";

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
	memset(e.vars, -1, sizeof(char) * 26);

	expr_t *a = parse_expr(test, sizeof test - 1);
	debug_print_expr(a, 0);

	printf("%d\n", e.count);
	printf("%s = %d\n", test, eval_expr(a, &e));

	func_t f = create_bool_func(a, &e);

	for(int i = 0; i < 26; i++)
	{
		if(e.vars[i] != -1)
			printf("%c: %d\n", e.vars[i] + 'A', i);
	}
	puts("");

	for(int i = 0; i < (1 << f.count); i++)
		printf("%d ", f.min_terms[i]);
	puts("");

	expr_t *t = reduce_bool_func(f);
	debug_print_expr(t, 0);

	delete_expr(a);

	return 0;
}
