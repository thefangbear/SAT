/*
	Basic SAT
	basic_sat.cpp
Author: Rui-Jie Fang
Date: June 2018
 */
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <unordered_set>
#include <utility>
//#define VERBOSE
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
							if (**prog!=')') err(); 
							++(*prog);
							printf("B(): S() returned, **prog=='\\0'? %d [**prog=%d]\n",**prog=='\0', **prog);
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
#ifdef VERBOSE
	printf("_eval: %d,%d\n",(bool) v1,(bool) v2);
#endif
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
typedef unsigned long long ull;
ull _table, _mark; 
unsigned _pos[50], p=0;

inline static void init_solve()
{ _table = _mark = 0; memset(_pos, 0, 50);}

inline static void _reg(char v)
{ _pos[v]=p; ++p; _mark |= (1ULL << v); }

inline static char _contains(char v)
{ return (_mark>>v) & 1ULL; }

inline static char _lookup(char v)
{ return (_table >> (_pos[v])) & 1ULL;}

inline static char to_printout(char x)
{
	switch (x) {
		case 0: return 'F';
		case 1: return 'T';
		default: return 'X';
	}
}

void print_solution(bexpr* expr)
{
	if (expr==NULL) return;
	while (expr->op != ';') {
		if (expr->n_expr)
			print_solution(expr->n_expr);
		if (expr->isfree) {
			printf("Variable [%c] := %c\n", expr->val, to_printout(_lookup(expr->val)));
		}
		expr = expr->expr;
	}
	if (expr->n_expr)
		print_solution(expr->n_expr);
	if (expr->isfree) {
		printf("Variable [%c] := %c\n", expr->val, to_printout(_lookup(expr->val)));
	}
#ifdef VERBOSE
	printf("print_solution(): all free variables printed, returning...\n");
#endif
	return;
}

bool _feval(bexpr* expr)
{
	bool r;
	char op = expr->op;
	if (expr->n_expr)
		r = _feval(expr->n_expr);
	else {
		if (expr->isfree) {
#ifdef VERBOSE
			printf("_feval(): encountered free variable %c, current value = %d\n", expr->val, (bool) _lookup(expr->val));
#endif
			r = expr->neg ? (bool) ~_lookup(expr->val) : (bool) _lookup(expr->val);
		} else
			r = expr->neg ? ~_cast(expr->val) : _cast(expr->val);
	}
	if (op == ';') return r;
	
	do {
		expr = expr->expr;
		bool p;
		if (expr->n_expr)
			p = _feval(expr->n_expr);
		else {
			if (expr->isfree) {
#ifdef VERBOSE
				printf("_feval(): encountered free variable %c, current value = %d\n", expr->val, (bool) _lookup(expr->val));
#endif
				p = expr->neg ? (bool) ~_lookup(expr->val) : (bool) _lookup(expr->val);
			} else 
				p = expr->neg ? ~ _cast(expr->val) : _cast(expr->val);
		}
#ifdef VERBOSE
		printf(" - p=%d, curr expr->val=%c\n", p, expr->val);
#endif
		r = _eval(r, p, op);
		op = expr->op;
	} while (op != ';');
#ifdef VERBOSE
	printf("_feval() result: %d\n", r);
#endif
	return r;
}

inline static void err_none()
{
	printf("err: SAT: nothing to solve.\n");
	exit(1);
}

unsigned _reg_free_vars(bexpr* expr)
{
	unsigned f = 0;
	while (expr->op != ';') {
		if (expr->n_expr) {
			f += _reg_free_vars(expr->n_expr);
		}
		if (expr->isfree) {
			if (!_contains(expr->val))
				++f;
#ifdef VERBOSE
			printf("reg(): registering free variable %c\n", expr->val);
#endif
			_reg(expr->val);
		}
		expr = expr->expr;
	}
	if (expr->n_expr)
		return f + _reg_free_vars(expr->n_expr);
	if (expr->isfree) {
		if (!_contains(expr->val))
			++f;
#ifdef VERBOSE
		printf("reg(): registering free variable %c\n", expr->val);
#endif
		_reg(expr->val);
	}
	return f;
}

void solve(bexpr* expr)
{
#ifdef VERBOSE
	printf("solve: init()\n");
#endif
	init_solve();
#ifdef VERBOSE
	printf("solve: regitering free variables\n");
#endif
	unsigned n = _reg_free_vars(expr);
//#ifdef VERBOSE
	printf("solve: got %u variables, solving [%llu possibilities]...\n", n, (unsigned long long)(1ULL<<n));
//#endif
	for(ull i = 0; i <= (1ULL<<n); ++i) {
		_table = i; // set the table to a generated bit pattern
		if (_feval(expr)) {
			printf(":::::: Viable solution :::::\n");
			print_solution(expr);
			printf("............................\n");
		}
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
//	printf("eval result is %d\n", eval(expr));
	printf(" === solving === \n");
	solve(expr);
	bexpr_free(expr);
	return 0;
}
