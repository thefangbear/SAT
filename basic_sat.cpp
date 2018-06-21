#include <cstdio>
#include <cstdlib>
#include <iostream>
using namespace std;
struct bexpr {
	char val;
	char op;
	char neg;
	char isfree;
	bexpr *expr;
	bexpr * n_expr;
};

inline static void err()
{
	printf("syntax error\n");
	exit(1);
}

inline static char* skip(char* s)
{
	while (*s == ' ' || *s == '\n' || *s == '\t') ++s;
	//printf("skip: @%c\n", *s);
	return s;
}

// grammar:
// <S>: <bool> <op> <S> | <bool> | <t>
// <bool>: '~' <bval> | <bval>
// <bval>: '1' | '0' | 't' | 'f' | 'T' | 'F'
// <op>: '^' | '&' | '|'

inline static bool is_free_variable(char c)
{
	return ((c >= 'a') && (c <= 'z')) || ((c >= 'A') && (c <= 'Z'));
}

void S(bexpr* expr, char** prog);
void B(bexpr* expr, char** prog)
{
	char* s = *prog;
	s = skip(s);
	if (!s || !(*s)) err(); // <bool> does not allow <t>
	printf("B(): @%c\n", *s);
	expr->isfree= 0;
	expr->neg = 0;
	switch (*s) {
		case '(': {
							++s;
							expr->n_expr = new bexpr;
							*prog=s;
							S(expr->n_expr, prog);
							++(*prog);
							printf("B(): S() returned, **prog=%d\n",**prog=='\0');
							return;
							break;
							}
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
							expr->n_expr = NULL;
							expr->val = *s;
							++s;
							break;
		default:
							if (is_free_variable(*s)) {
								expr->isfree = 1;
								expr->n_expr = NULL;
								expr->val = *s;
								++s;
								break;
							}
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
	if (!(*prog) || !(**prog) || (**prog==0) || (**prog=='\0') || (**prog==';')||(**prog==')')){\
			expr->op=';';\
			printf("T(): return\n");\
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
	printf("S(): parsing OP(), %c\n",**prog);
	OP(expr, prog);
	if (T(expr,prog)) err(); // strict check for next <S>, no term allowed here
	expr->expr = new bexpr;
	printf("S(): parsing S()\n");
	S(expr->expr, prog);
}

void RPN_print(bexpr * expr)
{
	if (expr->op == ';') {
		if (expr->n_expr==NULL)
			printf("%c", expr->val);
		else {
			printf("(");
			RPN_print(expr->n_expr);
			printf(")");
		} 
		return;
	} else if (expr->n_expr != NULL) {
		printf("(");
		printf("%c ", expr->op);
		RPN_print(expr->n_expr);
		if (expr->expr)
			RPN_print(expr->expr);
		printf(")");
	}
	printf("(");
	printf("%c ", expr->op);
	if (expr->neg) printf("%c",'~');
	printf("%c ", expr->val);
	if(expr->expr)
		RPN_print(expr->expr);
	printf(")");
}

inline static bool _cast(char v)
{
	switch (v) {
		case 't':
		case 'T':
		case '1':
		case 1:
			return 1;
		case 'f':
		case 'F':
		case '0':
		case 0:
			return 0;
		default:
			printf("err: _cast() failure, val = %c\n", v);
			exit(1);
	}
}

inline static bool _eval(char v1, char v2, char op)
{
	printf("_eval: %c,%c\n",v1,v2);
	bool b1 = _cast(v1), b2 = _cast(v2);
	switch (op) {
		case '^':
			return b1 ^ b2;
		case '|':
			return b1 | b2;
		case '&':
			return b1 & b2;
		default:
			printf("err: _eval() failure, op = %c\n", op);
			exit(1);
	}
}

bool eval(bexpr* expr)
{
	if (expr->op == ';') {
		if (expr->n_expr==NULL) {
			return expr->neg  ? ~_cast(expr->val) : _cast(expr->val);
		} else {
			return expr->neg ? (~eval(expr->n_expr)) : eval(expr->n_expr);
		}
	} else if (expr->n_expr != NULL) {
		bool p = eval(expr->n_expr);
		return _eval(expr->neg ? ~p : p, eval(expr->expr), expr->op);
	} else { // expr->n_expr == NULL, is val
		if (expr->isfree) {
			printf("err: eval(): cannot evaluate a boolean expression with a free variable\n");
			exit(1);
		}
		return _eval(expr->neg ? ~_cast(expr->val) : _cast(expr->val), eval(expr->expr), expr->op);
	}
}

void bexpr_free(bexpr* expr)
{
	bexpr* next;
	while ( expr->op != ';' )
	{
		if (expr->n_expr != NULL)
			bexpr_free(expr->n_expr);
		next = expr->expr;
		delete expr;
		expr = next;
	}
	delete expr;
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
	printf("eval result is %d\n", eval(expr));
	bexpr_free(expr);
	return 0;
}
