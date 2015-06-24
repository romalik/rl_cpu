int main() {
	int i = 0;
	int j = 0;
	char myString[] = "Hello from rlCPU!\n";
	char buf[] = "This string will contain num";
	
	char myString2[] = "And the second string!\n";
	puts("Try some text..\n");
	puts(myString);
	puts("Change some letters\n");
	myString[5] = '!';
	puts(myString);
	
	puts(myString2);


	puts(" 4 > 0? ");
	i = 4;
	if(i > 0) {
		puts("YES!\n");
	} else {
		puts("NO!\n");
	}

	puts(" 4 < 0? ");
	i = 4;
	if(i < 0) {
		puts("YES!\n");
	} else {
		puts("NO!\n");
	}



	puts(" -4 > 0? ");
	i = -4;
	if(i > 0) {
		puts("YES!\n");
	} else {
		puts("NO!\n");
	}

	puts(" -4 < 0? ");
	i = -4;
	if(i < 0) {
		puts("YES!\n");
	} else {
		puts("NO!\n");
	}



	puts("Try to put digit 0: ");
	i = 0;	
	putDigit(i);
	puts(" done\n");

	puts("Try to put digit -0: ");
	i = 0;	
	putDigit(-i);
	puts(" done\n");

	puts("Try to put digit 2: ");
	i = 2;	
	putDigit(i);
	puts(" done\n");


	puts("Try to put digit 2 - 0: ");
	j = 0;
	i = 2;	
	putDigit(i - 0);
	puts(" done\n");


	puts("Try to put digit 2 * 0: ");
	j = 0;
	i = 2;	
	putDigit(i * j);
	puts(" done\n");


	puts("Try to put digit 2%10: ");
	i = 2;	
	putDigit(i%10);
	puts(" done\n");

	puts("Try to put num 2%10: ");
	i = 2;	
	putNum(i%10);
	puts(" done\n");

	puts("Try to put num (i = 3) i * 2: ");
	i = 3;	
	putNum(i*2);
	puts(" done\n");

	puts("Try to put num 218: ");
	i = 2;	
	putNum(218);
	puts(" done\n");
	
	puts("Try to put num -356: ");
	i = 2;	
	putNum(-356);
	puts(" done\n");

    puts("Try to put digit -2: ");
	i = -2;	
	putDigit(i);
	puts(" done\n");



	puts("Try to write 2: ");
	i = 2;
	putNum(i);
	puts(" done\n");

	puts("Try to write -2: ");
	i = -2;
	putNum(i);
	puts(" done\n");

	puts("Try to write -(2): ");
	i = 2;
	putNum(-i);
	puts(" done\n");

	puts("Try to write -(-2): ");
	i = -2;
	putNum(-i);
	puts(" done\n");

	puts("Try to write 2*3: ");
	i = 2;	
	putNum(i*3);
	puts(" done\n");

	puts("Try to write (-2)*(-3): ");
	i = -2;	
	putNum(-3*i);
	puts(" done\n");

	puts("Try to write 0*3: ");
	i = 0;	
	putNum(i*3);
	puts(" done\n");

	puts(" 4 > 5? ");
	i = 4;
	if(i > 5) {
		puts("YES!\n");
	} else {
		puts("NO!\n");
	}

	puts(" 8 > 5? ");
	i = 8;
	if(i > 5) {
		puts("YES!\n");
	} else {
		puts("NO!\n");
	}



	puts("Try to write 5-3: ");
	i = 5;
	putNum(i-3);
	puts(" done\n");


	puts("Try to write 3-5: ");
	i = 3;
	putNum(i-5);
	puts(" done\n");


	puts("Try to write 5/3: ");
	i = 5;
	putNum(i/3);
	puts(" done\n");

	puts("Try to write 9/5: ");
	i = 9;
	putNum(i/5);
	puts(" done\n");


	puts("Try to write 9%5: ");
	i = 9;
	putNum(i%5);
	puts(" done\n");

	puts("Try to write 105%33: ");
	i = 105;
	putNum(i%33);
	puts(" done\n");

	puts("Try to write 2%10: ");
	i = 2;
	putNum(i%10);
	puts(" done\n");

	puts("Now let's try itoa. Try to write 150: ");
	i = 150;
	itoa(i, buf);
	puts(buf);
	puts(" done!\n");

	puts("Now let's try itoa. Try to write -50: ");
	i = -50;
	itoa(i, buf);
	puts(buf);
	puts(" done!\n");

	for(i = 1; i<=10; i++) {
		for(j = 1; j<=10; j++) {
			putNum(i*j);
			putc('\t');
		}
		putc('\n');
	}
	return 0;
}

