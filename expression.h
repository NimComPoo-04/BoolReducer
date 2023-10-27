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

	PAREN,		// (
	ERROR
};

typedef struct expr_t
{
	int complete;
	int type;
	union
	{
		struct {
			struct expr_t *lhs;	// left hand side of the op
			struct expr_t *rhs;	// right hand side of the op
		} and, or, nand, nor, xor, xnor;

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
void debug_print_expr_oneline(expr_t *d);
void delete_expr(expr_t *e);

// Evaluation of Expression
//
typedef struct
{
	uint32_t value;	// Values of the different variables stored as bits
	int count;	// holds the number of variables in system
	char vars[26];  // hashmap, index is offset by 'A',
			// stores the order of discovery
} env_t;

// NOTE: the variables are discovered during evaluation and not during parsing
// although it would probably make more sense to do it during parsing.
int eval_expr(expr_t *e, env_t *x);

#endif // _EXPRESSION_H_
