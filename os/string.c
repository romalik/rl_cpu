
int isdigit(char c) {
    return (c >= '0' && c <= '9');
}

int atoi(const char *s) {
    int res = 0;
    while (isdigit(*s)) {
        res = res * 10;
        res += (*s) - '0';
        s++;
    }
    return res;
}

int strcmp(void *s1, void *s2) {
    const unsigned char *c1 = (const unsigned char *)s1;
    const unsigned char *c2 = (const unsigned char *)s2;
    unsigned char ch;
    int d = 0;

    while (1) {
        d = (int)(ch = *c1++) - (int)*c2++;
        if (d || !ch)
            break;
    }

    return d;
}

int strlen(void *s) {
    char *p = (char *)s;
    int n = 0;
    while (*p) {
        p++;
        n++;
    }
    return n;
}

void reverse(char *s) {
    int i, j;
    char c;

    for (i = 0, j = strlen(s) - 1; i < j; i++, j--) {
        c = s[i];
        s[i] = s[j];
        s[j] = c;
    }
}

/* itoa:  convert n to characters in s */
void itoa(int n, char s[]) {
    int i, sign = 0;
    if (n < 0) { /* record sign */
        s[0] = '-';
        itoa(-n, s + 1);
        return;
    }
    i = 0;
    do { /* generate digits in reverse order */

        s[i++] = n % 10 + '0'; /* get next digit */
    } while ((n /= 10) > 0);   /* delete it */
    if (sign == 1) {
        s[i++] = '-';
    }
    s[i] = '\0';
    reverse(s);
}

int strcpy(unsigned int *dest, unsigned int *src) {
    do {
        *dest = *src;
        dest++;
        src++;
    } while (*src);
    return 0;
}

int memcpy(unsigned int *dest, unsigned int *src, int n) {
    int k = n;
    while (k--) {
        *dest = *src;
        dest++;
        src++;
    }
    return n;
}

int memcpy_r(unsigned int *dest, unsigned int *src, int n) {
    int k = n;
    while (k--) {
        *dest = *src;
        dest++;
        src++;
    }
    return n;
}
