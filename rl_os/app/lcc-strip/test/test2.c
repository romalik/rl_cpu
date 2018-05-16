int func1(int a) {
	return a+5;
}

long func2(long b) {
	return b + 12;
}

int main() {
	int a,b,c;
	a = 1;
	b = 2;
	a = a + func1(b);

	c = 100;
	func1(c);
	c = 200;
	func2(c);
}

