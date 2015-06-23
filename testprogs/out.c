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


int putNum(int a) {
	putc('0' + a);
	return 0;
}
int main() {
	char myString[] = "Hello from rlCPU!\n";
	int i = 0;

	char buf[100];
	
	char myString2[] = "And the second string!\n";
	puts("Try some text..\n");
	puts(myString);
	puts(myString2);

	puts("Try to write 2: ");
	i = 2;
	putNum(i);
	puts(" done\n");

	puts("Try to write 2*3: ");
	
	putNum(i*3);
	puts(" done\n");

	puts("Try to write 6/3: ");
	i = 6;
	putNum(i/3);
	puts(" done\n");

	puts("Try to write 16/5: ");
	i = 16;
	putNum(i/5);
	puts(" done\n");


	puts("Try to write 16%5: ");
	i = 16;
	putNum(i%5);
	puts(" done\n");

	puts("Now let's try itoa. Try to write 150: ");
	itoa(150, buf);
	puts(buf);
	puts(" done!\n");

	return 0;
}

