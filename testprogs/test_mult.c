int gimmeFive() {
	return 5;
}


int putc(char c) {
	*(char *)(0xffff) = c;
	return 0;
}

int putNum(int n) {
	if(n >= 0 && n < 10) {
		putc('0' + n);
	}
	return 0;

}

int main() {

	int a = 2;
	putNum(gimmeFive());
	putc('\n');

	putNum(a);
	a = a * 3;
	putc('\n');
	putNum(a);
	

	return 0;
}

