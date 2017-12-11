#ifndef __STRING_H
#define __STRING_H

int strlen(const void *s);
void reverse(char *s);
void itoa(int n, char s[]);
int memset(unsigned int *dest, const unsigned int val, int n);
int memcpy(unsigned int *dest, const unsigned int *src, int n);
//int memcpy_r(unsigned int *dest, const unsigned int *src, int n);
int strcpy(void *dest, const void *src);
char *strncpy(char *d, const char *s, unsigned int l);
int strcmp(const void *s1, const void *s2);
int atoi(const char *s);
int memcmp(const void *mem1, const void *mem2, int len);

#endif
