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


int main() {
	char myString[] = "Hello from rlCPU!\n";

	char myString2[] = "And the second string!\n";
	puts(myString);
	puts(myString2);

	return 0;
}

