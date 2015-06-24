
void itoa(int n, char s[]);

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


void putDigit(int a) {
	if(a < 0) {
		a = -a;
		putc('-');
	}
	putc('0' + a);
	
}

void putNum(int a) {
	char buffer[20];
	itoa(a, buffer);
	puts(buffer);
}

int strlen(char *s) {
	char * p = s;
	int n = 0;
	while(*p) {
		p++;
		n++;
	}
	return n;
}

 void reverse(char *s)
 {
     int i, j;
     char c;
 
     for (i = 0, j = strlen(s)-1; i<j; i++, j--) {
         c = s[i];
         s[i] = s[j];
         s[j] = c;

			

     }
 }

 /* itoa:  convert n to characters in s */
 void itoa(int n, char s[])
 {
     int i, sign = 0;
     if (n < 0) { /* record sign */
	s[0] = '-';
	itoa(-n, s+1);
	return;
     }
     i = 0;
     do {       /* generate digits in reverse order */
		 
         s[i++] = n % 10 + '0';   /* get next digit */
     } while ((n /= 10) > 0);     /* delete it */
     if (sign == 1) {
         s[i++] = '-';
     }
     s[i] = '\0';
     reverse(s);
 }

