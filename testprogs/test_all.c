#include "stdio.h"

void fail(int line) {
	printf("Failed on line %d\n", line);
}


int test_comp(int * success, int * all) {
	int a = 0;
	int b = 0;
	long al = 0;
	long bl = 0;
	unsigned int ua	= 0;
	unsigned int ub = 0;
	unsigned long ual = 0;
	unsigned long ubl = 0;

	a = 1; b = 2;
	if(a + b == 3) { (*success)++; } else {fail(__LINE__); }(*all)++;
	if(a + b > 2) { (*success)++; } else {fail(__LINE__); }(*all)++;
	if(a + b < 4) { (*success)++; } else {fail(__LINE__); }(*all)++;
	if(a + b > -2) { (*success)++; } else {fail(__LINE__); }(*all)++;
	if(a + b < 100000) { (*success)++; } else {fail(__LINE__); }(*all)++;
	if(a + b > -100000) { (*success)++; } else {fail(__LINE__); }(*all)++;
	if(a + b != 0) { (*success)++; } else {fail(__LINE__); }(*all)++;

	a = 5; b = -10;
	if(a + b == -5) { (*success)++; } else {fail(__LINE__); }(*all)++;
	if(a + b > -15) { (*success)++; } else {fail(__LINE__); }(*all)++;
	if(a + b < 100500) { (*success)++; } else {fail(__LINE__); }(*all)++;
	if(!(a + b > 100500)) { (*success)++; } else {fail(__LINE__); }(*all)++;
	if(a + b > -100500) { (*success)++; } else {fail(__LINE__); }(*all)++;
	if(a + b != 0) { (*success)++; } else {fail(__LINE__); }(*all)++;


	ua = 1; ub = 2;
	if(ua + ub == 3) { (*success)++; } else {fail(__LINE__); }(*all)++;
	if(ua + ub > 2) { (*success)++; } else {fail(__LINE__); }(*all)++;
	if(ua + ub < 4) { (*success)++; } else {fail(__LINE__); }(*all)++;
	if(ua + ub > -2) { (*success)++; } else {fail(__LINE__); }(*all)++;
	if(ua + ub < 100000) { (*success)++; } else {fail(__LINE__); }(*all)++;
	if(ua + ub > -100000) { (*success)++; } else {fail(__LINE__); }(*all)++;
	if(ua + ub != 0) { (*success)++; } else {fail(__LINE__); }(*all)++;


}



int main() {
	int success = 0;
	int all = 0;

	test_comp(&success, &all);


	printf("Success %d tests out of %d\n", success, all);
	return 0;
}
