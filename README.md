# SAT
This repository aims at providing a set of routines for parsing and solving simple boolean expressions in C/C++.

The parser we use is a simple recursive descent parser that uses the following grammar:
```
<S> : <B> <OP> <S> | <T> ;
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
