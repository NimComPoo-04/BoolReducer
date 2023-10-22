#ifndef _EXPRESSION_H_
#define _EXPRESSION_H_

#include <stdint.h>

enum
{
	VARIABLE,	// all alphabets allowed, case insensetive
	LITERAL,	// 0 1
	AND,		// .
	OR,		// +
	NOT,		// '
	PAREN		// (
};

typedef struct expr_t
{
	int type;
	union
	{
		struct {
			struct expr_t *lhs;	// left hand side of the op
			struct expr_t *rhs;	// right hand side of the op
		} and, or;

		struct {
			struct expr_t *lhs;	// only has a left hand side
		} not;

		char literal;	 // literal value
		char variable;	 // variable character

		struct {
			struct expr_t *eval;	// evaluation priority expression
		} paren;
	};
} expr_t;

expr_t *parse_expr(const char *str, size_t length);
void debug_print_expr(expr_t *e, int depth);
void delete_expr(expr_t *e);

// expr_t *init_expr(int type, ...);

// Evaluation of Expression
//
typedef struct
{
	uint32_t value;
	int count;
	char vars[26];
} env_t;

int eval_expr(expr_t *e, env_t *x);

#endif // _EXPRESSION_H_
