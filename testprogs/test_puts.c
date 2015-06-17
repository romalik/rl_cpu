
void puts(char *s) {
	while(*s) {
		putc(*s);
		s++;
	}
}

int main() {
	char myString[] = "Hello from rlCPU!";
	puts(myString);
	return 0;
}

