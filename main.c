#include <stdio.h>
#include <string.h>

#include "expression.h"
#include "function.h"

// const char test[] = "(x' . Y + x . Y')' . Z + (X' . Y + X . Y') . Z'";
// const char test[] = "A. B . C  + A . B . C' + A . B' . C";

/*
 * ABC + ABC' + AB'C
 * AB + AC
 * */

char buffer[1024] = {0};

int main(void)
{
	printf("Enter an Boolean Expression: ");
	fgets(buffer, sizeof buffer, stdin);

	expr_t *e = parse_expr(buffer, strlen(buffer) - 1);

	puts("== DEBUG ==");
	debug_print_expr(e, 0);

	env_t huh = {0};
	memset(huh.vars, -1, sizeof huh.vars);

	func_t f = create_func(e, huh);
	debug_print_func(&f);

	expr_t *d = reduce_func(&f);

	puts("== DEBUG ==");
	debug_print_expr(d, 0);

	puts("== OUTPUT ==");
	printf("%.*s = ", strlen(buffer) - 1, buffer);
	debug_print_expr_oneline(d);
	printf("\n");

	delete_function(&f);
	delete_expr(e);
	delete_expr(d);

	return 0;
}
