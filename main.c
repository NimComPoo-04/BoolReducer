#include <stdio.h>
#include <string.h>

#include "expression.h"
#include "function.h"
#include "reducer.h"

// const char test[] = "(x' . Y + x . Y')' . Z + (X' . Y + X . Y') . Z'";
// const char test[] = "A. B . C  + A . B . C' + A . B' . C";

/*
 * ABC + ABC' + AB'C
 * AB + AC
 * */

char buffer[1024] = {0};

int main(void)
{
	printf("\nEnter an Boolean Expression: ");
	scanf("%[^\n]", buffer, stdin);

	expr_t *e = parse_expr(buffer, strlen(buffer));

	puts("\n== INPUT EXPRESSION ==");
	debug_print_expr(e, 0);

	env_t huh = {0};
	memset(huh.vars, -1, sizeof huh.vars);

	func_t f = create_func(e, huh);
	if(f.count > 10)
		printf("WARNING: The number of variables are too high.\n");

	debug_print_func(&f);

	/*
	term_list_t t = terms_copy(&f);
	debug_print_term_list(t);

	puts("\n== REDUCED TERMS ==");
	reduce_term_list(&t);
	*/

	expr_t *d = reduce_func(&f);

	puts("\n== OUTPUT EXPRESSION ==\n");
	debug_print_expr(d, 0);

	puts("\n== REDUCED EXPRESSION ==\n");
	debug_print_expr_oneline(d);
	puts("\n");

	delete_function(&f);
	delete_expr(e);
	delete_expr(d);

	return 0;
}
