#include <iostream>
#include <cstdio>
#include <stack>
using namespace std;

// iterative recursion demo

// standard recursion
const unsigned K = 10;

void A(unsigned N)
{
	if (N >= K) {
		printf("A(): K reached\n");
		return;
	}
	printf("A(): N=%u\n", N);
	A(N + 1);
	A(N + 2);
	A(N + 3);
}

void A_stk()
{
	stack<unsigned> stk;
	unsigned N = 0;
	stk.push(N);
	do {
		N = stk.top();
		stk.pop();
		if (N >= K) {
			printf("A(): K reached\n");
			continue;
		}
		printf("A(): N=%u\n", N);
		stk.push(N+3);
		stk.push(N+2);
		stk.push(N+1);
	} while (!stk.empty());
}

int main(int argc, char ** argv)
{
	printf("%d\n", argc);
	if (argc == 2)
		A(0);
	else
		A_stk();
	return 0;
}
