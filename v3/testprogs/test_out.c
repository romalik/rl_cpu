int putc(char c) {
	*(char *)(0xffff) = c;
	return 0;
}

int  puts(char *s) {
	while(*s) {
		putc(*s);
		s++;
	}
	return 0;
}

char myString[] = "Hello from rlCPU!";

int main() {
	puts(myString);

	return 0;
}

