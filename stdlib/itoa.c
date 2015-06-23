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

			putNum(i);
			puts(" <-> ");
			putNum(j);

			puts("  Reverse iteration ");
			puts(s);
			putc('\n');
			

     }
	puts("result:\n");
	puts(s);
	puts("\nok\n");
 }

 /* itoa:  convert n to characters in s */
 void itoa(int n, char s[])
 {
     int i, sign;
     if ((sign = n) < 0)  /* record sign */
         n = -n;          /* make n positive */
     i = 0;
     do {       /* generate digits in reverse order */
		 putc(n%10 + '0'); putc('\n');
         s[i++] = n % 10 + '0';   /* get next digit */
     } while ((n /= 10) > 0);     /* delete it */
/*     if (sign < 0)
         s[i++] = '-';
*/
     s[i] = '\0';
	puts("len: ");
	putNum(strlen(s));
	puts("\n");
	puts(s);
	puts("\nreverse:\n");
     reverse(s);
	puts(s);
	puts("\n");
 }

