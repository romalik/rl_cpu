int putc(char c) {
	*(char *)(0xffff) = c;
	return 0;
}

int main() {
	int i = 0;
	for(i = 0; i<'z' - 'a'; i++) {
		putc('a' + i);
	}
	while(1) {}
	return 0;
}

