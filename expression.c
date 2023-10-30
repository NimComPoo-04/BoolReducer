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

		case PAREN:
			e->type = PAREN;
			e->paren.eval = va_arg(v, expr_t *);
			break;

		case ERROR:
			e->type = ERROR;
			break;
	}

	va_end(v);
	return e;
}

static void no_space_op(expr_t *a, expr_t *b)
{
	if((a->type == AND || a->type == OR) &&
			(a->and.rhs->type != AND && a->and.rhs->type != OR))
	{
		a->and.rhs = init_expr(AND, a->and.rhs, b);
	}
	else
		no_space_op(a->and.rhs, b);
}

void delete_expr(expr_t *e)
{
	if(!e) return;

	if(e->type == AND || e->type == OR)
	{
		delete_expr(e->and.lhs);
		delete_expr(e->and.rhs);
	}
	if(e->type == NOT || e->type == PAREN)
	{
		delete_expr(e->and.lhs);
	}

	free(e);
}

static expr_t *__parse_expr(expr_t *e,
		const char *str, size_t length, size_t *current);
static expr_t *__parse_paren(const char *str, size_t length, size_t *current)
{
	expr_t *head = 0;
	char c = 0;

	while(*current < length)
	{
		int pst = *current;
		c = next_token(str, length, current);
		if(c == ')')
			return head;
		*current = pst;

		expr_t *e = __parse_expr(head, str, length, current);

		if(head && (e->type == VARIABLE || e->type == LITERAL || e->type == PAREN))
		{
			if(head->type == VARIABLE || head->type == LITERAL || head->type == PAREN)
				head = init_expr(AND, head, e);
			else
				no_space_op(head, e);
		}
		else
			head = e;
	}

	fprintf(stderr, "')' is expected at the end of expression.\n");
	exit(1);
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
					e->or.rhs = __parse_expr(e->or.rhs, str, length, current);
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

		case '(':
			return init_expr(PAREN, __parse_paren(str, length, current));
	}

	fprintf(stderr, "Undefined Token (%c : %d) Encountered!\n", c, c);
	exit(1);
}

void debug_print_expr_oneline(expr_t *e)
{
	if(!e) return;

	switch(e->type)
	{
		case VARIABLE:
			printf("%c", e->variable);
			break;

		case LITERAL:
			printf("%c", e->literal);
			break;

		case AND:
			debug_print_expr_oneline(e->and.lhs);
//			printf(" . ");
			debug_print_expr_oneline(e->and.rhs);
			break;

		case OR:
			debug_print_expr_oneline(e->or.lhs);
			printf(" + ");
			debug_print_expr_oneline(e->or.rhs);
			break;

		case NOT:
			debug_print_expr_oneline(e->not.lhs);
			printf("'");
			break;

		case PAREN:
			printf("( ");
			debug_print_expr_oneline(e->paren.eval);
			printf(" )");
			break;
	}
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

		case PAREN:
			printf("(\n");
			debug_print_expr(e->paren.eval, depth + 1);
			printf("%.*s)\n", depth, SPACES);
			break;
	}
}

expr_t *parse_expr(const char *str, size_t length)
{
	expr_t *head = NULL;
	size_t current = 0;

	do
	{
		expr_t *tmp = __parse_expr(head, str, length, &current);
		if(head && (tmp->type == VARIABLE || tmp->type == LITERAL || tmp->type == PAREN))
		{
			if(head->type == VARIABLE || head->type == LITERAL || head->type == PAREN)
				head = init_expr(AND, head, tmp);
			else
				no_space_op(head, tmp);
		}
		else
			head = tmp;
	} while(current < length);

	return head;
}

// walk the tree bois!!
int eval_expr(expr_t *e, env_t *x)
{
	if(!e)
	{
		fprintf(stderr, "Error during evaluation.\n");
		exit(1);
	}

	switch(e->type)
	{
		case VARIABLE:
			if(x->vars[e->variable - 'A'] == -1)
			{
				x->vars[e->variable - 'A'] = x->count;
				x->count++;
			}
			return (x->value >> x->vars[e->variable - 'A']) & 1;

		case LITERAL:
			return e->literal - '0';

		case NOT:
			return !eval_expr(e->not.lhs, x);

		case AND:
			{
				int e1 = eval_expr(e->and.lhs, x);
				int e2 = eval_expr(e->and.rhs, x);
				return e1 && e2;
			}

		case OR:
			{
				int e1 = eval_expr(e->and.lhs, x);
				int e2 = eval_expr(e->and.rhs, x);
				return e1 || e2;
			}

		case PAREN:
			return eval_expr(e->paren.eval, x);

		default:
			return -1;
	}
}
