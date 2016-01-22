#ifndef __STRING_H
#define __STRING_H

int strlen(char *s);
void reverse(char *s);
void itoa(int n, char s[]);
int memcpy(unsigned int *dest, unsigned int *src, int n);
int memcpy_r(unsigned int *dest, unsigned int *src, int n);
int strcpy(char *dest, char *src);
int strcmp(const char *s1, const char *s2);
int atoi(const char *s);

#endif
