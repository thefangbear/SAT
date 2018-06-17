#include <cstdio>
#include <cstdlib>
#include <iostream>
using namespace std;
struct bexpr {
	char val;
	char op;
	char neg;
	bexpr *expr;
};

void err()
{
	printf("syntax error\n");
	exit(1);
}

char* skip(char* s)
{
	//while (*s == ' ' || *s == '\n' || *s == '\t') ++s;
	//printf("skip: @%c\n", *s);
	return s;
}

// grammar:
// <S>: <bool> <op> <S> | <bool> | <t>
// <bool>: '~' <bval> | <bval>
// <bval>: '1' | '0' | 't' | 'f' | 'T' | 'F'
// <op>: '^' | '&' | '|'

void B(bexpr* expr, char** prog)
{
	char* s = *prog;
	s = skip(s);
	if (!s || !(*s)) err(); // <bool> does not allow <t>
	printf("B(): @%c\n", *s);
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
	printf("OP(): @%c\n", *s);
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

#define _T(expr,prog) \
	if (!(*prog) || !(**prog) || (**prog==';')){\
			expr->op=';';\
			return 1;}

bool T(bexpr* expr, char** prog)
{
	_T(expr,prog);
	*prog = skip(*prog);
	_T(expr,prog);
	return 0;
}

void S(bexpr* expr, char** prog)
{
	printf("S(): check\n");
	if (T(expr,prog)) return; // the <t> case
	printf("S(): parsing B()\n");
	B(expr, prog);
	if (T(expr,prog)) return;
	printf("S(): parsing OP()\n");
	OP(expr, prog);
	if (T(expr,prog)) err(); // strict check for next <S>, no term allowed here
	expr->expr = new bexpr;
	printf("S(): parsing S()\n");
	S(expr->expr, prog);
}

void RPN_print(bexpr * expr)
{
	if (expr->op == ';') {
		printf("%c", expr->val);
		return;
	}
	printf("(");
	printf("%c ", expr->op);
	if (expr->neg == '~') printf("%c",expr->neg);
	printf("%c ", expr->val);
	if(expr->expr)
		RPN_print(expr->expr);
	printf(")");
}

int main()
{
	char * s;
	size_t len;
	cin >> len;
	s = (char*) malloc(len);
	memset(s, '\0', len);
	scanf("%s", s);
	++s;
	printf("input: %s\n", s);
	bexpr *expr = new bexpr;
	S(expr, &s);
	RPN_print(expr);
	puts("");
	return 0;
}
