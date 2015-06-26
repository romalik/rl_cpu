int main() {
	puts("Integers\n");
	{
	int a = 20;
	int b = 12;

	if( a >= b ) {
		puts("20 >= 12\n");

	} else {
		puts("20 < 12\n");
	}

	a = 1;
	b = 2;

	if( a >= b ) {
		puts("1 >= 2\n");

	} else {
		puts("1 < 2\n");
	}

	a = 123;
	b = 123;

	if( a >= b ) {
		puts("123 >= 123\n");

	} else {
		puts("123 < 123\n");
	}
	}
	puts("Longs\n");
	{
	long a = 20;
	long b = 12;

	if( a >= b ) {
		puts("20 >= 12\n");

	} else {
		puts("20 < 12\n");
	}

	a = 1;
	b = 2;

	if( a >= b ) {
		puts("1 >= 2\n");

	} else {
		puts("1 < 2\n");
	}

	a = 123;
	b = 123;

	if( a >= b ) {
		puts("123 >= 123\n");

	} else {
		puts("123 < 123\n");
	}
	}
	return 0;


}
