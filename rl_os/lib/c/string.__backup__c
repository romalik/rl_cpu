#include <string.h>
#include <sys/types.h>

char *strrchr(const char *s, int c)
{
        const char *p = s + strlen(s);

        /* For null it's just like strlen */
        if (c == '\0')
                return (char *)p;
        while (p != s) {
                        if (*--p == c)
                                return (char *)p;
                }
        return NULL;
}

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

int strcmp(const void *s1, const void *s2) {
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

int strlen(const void *s) {
    const char *p = (const char *)s;
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

void* strcpy(void *dest_, const void *src_) {
    unsigned int *dest = (unsigned int *)dest_;
    unsigned int *src = (unsigned int *)src_;
    while (*src) {
        *dest = *src;
        dest++;
        src++;
    };
    *dest = *src;
    return dest_;
}

int memset(void *dest_, unsigned int val, int n) {
    unsigned int *dest = (unsigned int *)dest_;
    int k = n;
    while (k--) {
        *dest = val;
        dest++;
    }
    return n;
}

int memcpy(void *dest_, const void *src_, int n) {
    unsigned int *dest = (unsigned int *)dest_;
    unsigned int *src = (unsigned int *)src_;
    int k = n;
    while (k--) {
        *dest = *src;
        dest++;
        src++;
    }
    return n;
}

int memcpy_r(void *dest_, const void *src_, int n) {
    unsigned int *dest = (unsigned int *)dest_;
    unsigned int *src = (unsigned int *)src_;
    int k = n;
    while (k--) {
        *dest = *src;
        dest++;
        src++;
    }
    return n;
}
