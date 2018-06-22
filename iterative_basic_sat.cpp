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
#include <stack>
#include <utility>
//#define VERBOSE
//#define MEMINFO
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
// <bool>: '~' <bval> | <bval> | '(' <S> ')'
// <bval>: '1' | '0' | 't' | 'f' | 'T' | 'F'
// <op>: '^' | '&' | '|'

inline static bool is_free_variable(char c)
{
	return ((c >= 'a') && (c <= 'z')) || ((c >= 'A') && (c <= 'Z'));
}

void S(bexpr* expr, char** prog);
void B(bexpr* expr, char** prog)
{
#ifdef MEMINFO
	printf("addr *prog = %p\n", *prog);
#endif
	*prog = skip(*prog);
	if (!(*prog) || !(**prog)) err(); // <bool> does not allow <t>
	printf("B(): @%c\n", **prog);
	expr->isfree= 0;
	expr->neg = 0;
	switch (**prog) {
		case '(': {
							++(*prog);
							expr->n_expr = new bexpr;
							S(expr->n_expr, prog);
							++(*prog);
							printf("B(): S() returned, **prog=%d\n",**prog=='\0');
							return;
							break;
							}
		case '~': {
							expr->neg = 1;
							++(*prog);
							}
		case '1':
		case '0':
		case 't':
		case 'f':
		case 'T':
		case 'F':
							expr->n_expr = NULL;
							expr->val = **prog;
							++(*prog);
							break;
		default:
							if (is_free_variable(**prog)) {
								expr->isfree = 1;
								expr->n_expr = NULL;
								expr->val = **prog;
								++(*prog);
								break;
							}
							err();
	}
}

void OP(bexpr* expr, char** prog)
{
#ifdef MEMINFO
	printf("addr *prog = %p\n", *prog);
#endif
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
#ifdef MEMINFO
	printf("addr *prog = %p\n", *prog);
#endif
	printf("S(): check\n");
	if (T(expr,prog)) return; // the <t> case
	printf("S(): parsing B()\n");
	B(expr, prog);
	printf("S(): next: %d\n", **prog);
	if (T(expr,prog)) return;
	printf("S(): parsing OP(), %d\n",**prog);
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
char _table[1000];
inline static void init_solve()
{ for(unsigned i = 0; i < 1000; ++i) _table[i] = 2; }
inline static void _set(char var, char val)
{ _table[var] = val; }
inline static char _lookup(char var)
{ return _table[var]; }
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
	if (expr->op == ';') {
#ifdef VERBOSE
		printf("_feval(): reached end of expression\n");
#endif
		if (expr->n_expr) {
			return _feval(expr->n_expr);
		} else {
			if (expr->isfree) {
#ifdef VERBOSE
				printf("_feval(): lookup variable %c = %d\n", expr->val, (bool) _lookup(expr->val));
#endif
				return (bool) _lookup(expr->val);
			} else 
				return _cast(expr->val);
		}
	} else {
		if (expr->n_expr) {
			bool p = _feval(expr->n_expr);
			return _eval(p, _feval(expr->expr), expr->op);
		} else {
#ifdef VERBOSE
			printf("_feval(): @ variable %c\n", expr->val);
#endif
			if (expr->isfree) {
#ifdef VERBOSE
				printf("_feval(): lookup variable %c = %d\n", expr->val, (bool) _lookup (expr->val));
#endif
				bool f = (bool) _lookup(expr->val);
				return _eval(f, _feval(expr->expr), expr->op);
			} else {
				return _eval(_cast(expr->val), _feval(expr->expr), expr->op);
			}
		}
	}
}

inline static void err_none()
{
	printf("err: SAT: nothing to solve.\n");
	exit(1);
}

void _register_free_variables(bexpr* expr, unordered_set<char>& vs)
{
	if (expr->op == ';') {
		if (expr->n_expr) {
			_register_free_variables(expr->n_expr, vs);
#ifdef VERBOSE
			printf("reg(): end reached, vs.size() == %lu\n", vs.size());
#endif
			if (vs.size() == 0) err_none();
		} else {
			if (expr->isfree && (vs.find(expr->val) == vs.end())) {
				vs.insert(expr->val);
			}
#ifdef VERBOSE
			printf("reg(): end reached, vs.size() == %lu\n", vs.size());
#endif
			if (vs.size() == 0) err_none();
		}
	} else {
		if (expr->n_expr) {
			_register_free_variables(expr->n_expr, vs);
		} 
		if (expr->isfree && (vs.find(expr->val) == vs.end())) {
#ifdef VERBOSE
			printf("reg(): free variable found [%c]\n", expr->val);
#endif
			vs.insert(expr->val);
		}
		_register_free_variables(expr->expr, vs);
	}
}

void _backtrack(bexpr* expr, unordered_set<char>& vs, unordered_set<char>::iterator ptr)
{
	if (ptr == vs.end()) {
#ifdef VERBOSE
		printf("_backtrack(): size %lu reached, evaling...\n", vs.size());
#endif
		if (_feval(expr)) {
#ifdef VERBOSE
			printf("_backtrack(): found correct solution, printing out...\n");
#endif
			printf("::::: Viable Solution :::::\n");
			print_solution(expr);
			printf("..... ............... .....\n");
		}
		return;
	}
	unordered_set<char>::iterator optr = ptr;
	++ptr;
	_set(*optr, 1);
	_backtrack(expr, vs, ptr);
	_set(*optr, 0);
	_backtrack(expr, vs, ptr);
}

void _backtrack_iterative(bexpr* expr, unordered_set<char>& vs)
{
	pair<unordered_set<char>::iterator, bool> arg;
	unordered_set<char>::iterator optr;
	stack< pair<unordered_set<char>::iterator, bool> > stk;
	stk.push(make_pair<unordered_set<char>::iterator, bool>(vs.begin(), 0));
	stk.push(make_pair<unordered_set<char>::iterator, bool>(vs.begin(), 1));
	do {
		arg = stk.top();
		stk.pop();
		//printf("iter(): Variable %c, State %d\n", *(arg.first), arg.second);
		_set(*(arg.first), arg.second);	
		optr = arg.first;
		++optr;
		// do check here to avoid printing out identical solution twice
		// nasty hack for STL behaviour
		if (optr == vs.end()) {
#ifdef VERBOSE
			printf("_backtrack(): size %lu reached, evaling...\n", vs.size());
#endif
			if (_feval(expr)) {
#ifdef VERBOSE
				printf("_backtrack(): found correct solution, printing out...\n");
#endif
				printf("::::: Viable Solution :::::\n");
				print_solution(expr);
				printf("..... ............... .....\n");
			}
			continue;
		} else {
			stk.push(make_pair<unordered_set<char>::iterator, bool>(optr, 0));
			stk.push(make_pair<unordered_set<char>::iterator, bool>(optr, 1));
		}
	} while (!stk.empty());
}

inline static void solve(bexpr* expr)
{
#ifdef VERBOSE
	printf("solve(): init_solve\n");
#endif
	init_solve();
	unordered_set<char> free_vars;
#ifdef VERBOSE
	printf("solve(): finding free variables\n");

	for(char x : free_vars)
		printf("[Free Variable Found]: %c\n", x);
#endif
	_register_free_variables(expr, free_vars);
#ifdef VERBOSE
	printf("solve(): free vars found, backtracking...\n");
#endif 
//	_backtrack(expr, free_vars, free_vars.begin());
	_backtrack_iterative(expr, free_vars);
#ifdef VERBOSE
	printf("solve(): backtracking done.\n");
#endif
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
	char * s, * sp;
	size_t len;
	cin >> len;
	sp = s = (char*) malloc(len);

	memset(s, '\0', len);
	scanf("%s", s);
//	++s;
	printf("input: %s\n", s);
#ifdef MEMINFO
	printf("addr s = %p, addr s_end = %p\n", s, s + strlen(s) + 1);
	printf("s_end=%c %c\n", *(s+strlen(s)), *(s+strlen(s)+1));
#endif
	bexpr *expr = new bexpr;
	S(expr, &s);
	RPN_print(expr);
	puts("");
//	printf("eval result is %d\n", eval(expr));
	printf(" === solving === \n");
	solve(expr);
	bexpr_free(expr);
	free(sp);
	return 0;
}
