#ifndef __STRING_H
#define __STRING_H

int strlen(void *s);
void reverse(char *s);
void itoa(int n, char s[]);
int memset(unsigned int *dest, unsigned int val, int n);
int memcpy(unsigned int *dest, unsigned int *src, int n);
int memcpy_r(unsigned int *dest, unsigned int *src, int n);
int strcpy(char *dest, char *src);
int strcmp(void *s1, void *s2);
int atoi(const char *s);

#endif
