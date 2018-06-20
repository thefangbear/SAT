# SAT
This repository aims at providing a set of routines for parsing and solving simple boolean expressions in C/C++.

The parser we use is a simple recursive descent parser that uses the following grammar:
```
<S> : <B> <OP> <S> | <B> | <T> ;
<T> : ;
<B> : '~' <BV> | <BV> ;
<BV> : '0' | '1' | 't' | 'f' | 'T' | 'F' ;
<OP> : '&' | '^' | '|' ;
```

For solving SAT we add `'x'` to `<BV>` to denote a free variable.

The representation of boolean expressions follow the idea of a binary expression tree.

We have the following structure
```
typedef struct _bexpr {
	char val;
	char op;
	char neg;
	struct _bexpr expr;
} bexpr;
```
denote a valid expression. Evaluation proceeds in strict left-to-right manner.

## Development status

Currently testing and revising the parser for boolean expressions.

 - Step 1: Write a parser that supports parsing non-parenthesized boolean expressions (done, `sat_parser.cpp`)
 - Step 2: Write a parser that supports parenthesized boolean expressions (done, `parser.cpp`)
 - Step 3: Write a parser that evaluates boolean expressions (done, `parser.cpp`)
 - Step 4: Write a parser that handles free-formed expressions (working on it, `constraint_parser.cpp`)
 - Step 5: Implement a basic backtracking-based SAT solving algorithm
 - Step 6: Implement a more advanced SAT solving algorithm

## Author
	Rui-Jie Fang
