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

## Current solvers
We currently have three kinds of solvers, all implementing the basic binary backtrack method.

*Binary-recursive Backtrack Solver*: The solver uses the basic binary-recursive backtracking algorithm. Running time is O(N 2^k).

*Binary-iterative Backtrack Solver*: This solver uses an iterative approach to generate all 2^k solutions down the tree.

*Binary Stack-recursive Backtrack Solver*: (TODO), this solver uses a stack to explicitly handle the recursion.

## Development status

Currently testing and revising the parser for boolean expressions.

	- Step 1: Write a parser that supports parsing non-parenthesized boolean expressions (done, `sat_parser.cpp`)
	- Step 2: Write a parser that supports parenthesized boolean expressions (done, `parser.cpp`)
	- Step 3: Write a parser that evaluates boolean expressions (done, `parser.cpp`)
	- Step 4: Write a parser that handles free-formed expressions (done, `constraint_parser.cpp`)
	- Step 5: Implement a basic backtracking-based SAT solving algorithm (done, `basic_sat.cpp`)
	- Step 5.5: Implement an iterative version of `_backtrack()` in `basic_sat.cpp` to make it prettier (done, `sat.cpp`)
	- Step 6: Implement a more advanced SAT solving algorithm

## Example Usage
Enter the expected (maximum) length of the input expression on the first line, and the expression on the second line.
```
50
a&(b|c^d)
input: a&(b|c^d)
S(): check
S(): parsing B()
B(): @a
S(): parsing OP(), &
OP(): @&
S(): parsing S()
S(): check
S(): parsing B()
B(): @(
S(): check
S(): parsing B()
B(): @b
S(): parsing OP(), |
OP(): @|
S(): parsing S()
S(): check
S(): parsing B()
B(): @c
S(): parsing OP(), ^
OP(): @^
S(): parsing S()
S(): check
S(): parsing B()
B(): @d
T(): return
B(): S() returned, **prog=='\0'? 1 [**prog=0]
T(): return
(& a ((| b (^ c d))))
 === solving ===
solve: got 4 variables, solving [16 possibilities]...
:::::: Viable solution :::::
Variable [a] := T
Variable [b] := T
Variable [c] := F
Variable [d] := F
............................
:::::: Viable solution :::::
Variable [a] := T
Variable [b] := F
Variable [c] := T
Variable [d] := F
............................
:::::: Viable solution :::::
Variable [a] := T
Variable [b] := T
Variable [c] := T
Variable [d] := F
............................
:::::: Viable solution :::::
Variable [a] := T
Variable [b] := F
Variable [c] := F
Variable [d] := T
............................

```

## Author
	Rui-Jie Fang
