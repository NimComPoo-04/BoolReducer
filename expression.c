#include <ctype.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>

#include "expression.h"

static char next_token(const char *str, size_t length, size_t *current)
{
	if(*current >= length)
		return 0;
	while(*current < length && isspace(str[*current]))
		*current += 1;
	char c = str[*current];
	*current += 1;
	return c;
}

static expr_t *init_expr(int type, ...)
{
	va_list v;
	va_start(v, type);
	expr_t *e = calloc(sizeof(expr_t), 1);

	switch(type)
	{
		case VARIABLE:
			e->type = VARIABLE;
			e->variable = va_arg(v, int);
			break;

		case LITERAL:
			e->type = LITERAL;
			e->literal = va_arg(v, int);
			break;

		case AND:
			e->type = AND;
			e->and.lhs = va_arg(v, expr_t *);
			e->and.rhs = va_arg(v, expr_t *);
			break;

		case OR:
			e->type = OR;
			e->or.lhs = va_arg(v, expr_t *);
			e->or.rhs = va_arg(v, expr_t *);
			break;

		case NOT:
			e->type = NOT;
			e->not.lhs = va_arg(v, expr_t *);
			break;
	}

	va_end(v);
	return e;
}

void delete_expr(expr_t *e)
{
	if(!e) return;

	if(e->type == AND || e->type == OR)
	{
		delete_expr(e->and.lhs);
		delete_expr(e->and.rhs);
	}
	if(e->type == NOT)
	{
		delete_expr(e->and.lhs);
	}

	free(e);
}

static expr_t *__parse_expr(expr_t *e,
		const char *str, size_t length, size_t *current)
{
	char c = next_token(str, length, current);

	// Variable
	if(isalpha(c))
		return init_expr(VARIABLE, toupper(c));

	switch(c)
	{
		case '0': case '1':
			return init_expr(LITERAL, toupper(c));

		case '\'':
			switch(e->type)
			{
				case AND:
					*current -= 1; // unreading a token
					e->and.rhs = __parse_expr(e->and.rhs, str, length, current);
					return e;

				case OR:
					*current -= 1; // unreading a token
					e->and.rhs = __parse_expr(e->and.rhs, str, length, current);
					return e;
				default:
					return init_expr(NOT, e);
			}

		case '.':
			switch(e->type)
			{
				case OR:
					*current -= 1; // unreading a token
					e->or.rhs = __parse_expr(e->or.rhs, str, length, current);
					return e;
				default:
					return init_expr(AND, e, __parse_expr(e, str, length, current));
			}

		case '+':
			return init_expr(OR, e, __parse_expr(e, str, length, current));
			break;

	}

	fprintf(stderr, "Undefined Token Encountered!");
	exit(1);
}

const char SPACES[] = "\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t";
void debug_print_expr(expr_t *e, int depth)
{
	if(!e) return;

	printf("%.*s", depth, SPACES);
	switch(e->type)
	{
		case VARIABLE:
			printf("%c\n", e->variable);
			break;

		case LITERAL:
			printf("%c\n", e->literal);
			break;

		case AND:
			printf(".\n");
			debug_print_expr(e->and.lhs, depth + 1);
			debug_print_expr(e->and.rhs, depth + 1);
			break;

		case OR:
			printf("+\n");
			debug_print_expr(e->and.lhs, depth + 1);
			debug_print_expr(e->and.rhs, depth + 1);
			break;

		case NOT:
			printf("'\n");
			debug_print_expr(e->and.lhs, depth + 1);
			break;
	}
}

expr_t *parse_expr(const char *str, size_t length)
{
	expr_t *head = NULL;
	size_t current = 0;

	do
	{
		head = __parse_expr(head, str, length, &current);
	} while(current < length);

	return head;
}

// walk the tree bois!!
int eval_expr(expr_t *e, env_t *x)
{
	if(!e)
		return -1;

	switch(e->type)
	{
		case VARIABLE:
			if(x->vars[e->variable - 'A'] == -1)
				x->vars[e->variable - 'A'] = 0;
			return x->vars[e->variable - 'A'];

		case LITERAL:
			return e->literal - '0';

		case NOT:
			return !eval_expr(e->not.lhs, x);

		case AND:
			return eval_expr(e->and.lhs, x) && eval_expr(e->and.rhs, x);

		case OR:
			return eval_expr(e->or.lhs, x) || eval_expr(e->or.rhs, x);

		case PAREN:
			return eval_expr(e->paren.eval, x);

		default:
			return -1;
	}
}
