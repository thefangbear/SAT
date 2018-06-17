#include <cstdio>
#include <cstdlib>
#include <iostream>
using namespace std;
typedef struct _bexpr {
	char val;
	char op;
	char neg;
	struct _bexpr expr;
} bexpr;

void err()
{
	printf("syntax error\n");
	exit(1);
}

char* skip(char* s)
{
	while (*s == ' ' || *s == '\n' || *s == '\t') ++s;
	return s;
}

// grammar:
// <S>: <bool> <op> <S> | <t>
// <bool>: '~' <bval> | <bval>
// <bval>: '1' | '0' | 't' | 'f' | 'T' | 'F'
// <op>: '^' | '&' | '|'

void B(bexpr* expr, char** prog)
{
	char* s = *prog;
	s = skip(s);
	if (!s || (*s)) err(); // <bool> does not allow <t>
	switch (*s) {
		case '~': {
							expr->neg = 1;
							++s;
							}
		case '1':
		case '0':
		case 't':
		case 'f':
		case 'T':
		case 'F':
							expr->val = *s;
							++s;
							break;
		default:
							err();
	}
	*prog = s;
}

void OP(bexpr* expr, char** prog)
{
	char *s = *prog;
	s = skip(s);
	if (!s || !(*s)) err(); // <op> does not allow <term> either
	switch (*s) {
		case '^':
		case '&':
		case '|':
			expr->op = *s;
			++s;
			break;
		default:
			err();
	}
	*prog = s;
}

_Bool T(bexpr* expr, char** prog)
{
	if (!(*prog) || !(**prog)) {
		expr->val = 0;
		expr->expr = NULL;
		expr->op = ';';
		return 1;
	}
	*prog = skip(*prog);
	if (!(*prog) || !(**prog)) {
		expr->val = 0;
		expr->expr = NULL;
		expr->op = ';';
		return 1;
	}
	return 0;
}

void S(bexpr* expr, char** prog)
{
	if (T(expr,prog)) return; // the <t> case
	B(expr, prog);
	OP(expr, prog);
	expr->expr = new bexpr;
	S(expr->expr, prog);
}

int main()
{
	return 0;
}
