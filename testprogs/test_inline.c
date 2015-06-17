int sum(int a, int b) {
	return a + b;
}

int main() {
	int a = 0;
	_asm {
	"blah - inlined asm"
	};
	a = sum(2, 3);
	asm("blah2 %a");
	return 0;

}
