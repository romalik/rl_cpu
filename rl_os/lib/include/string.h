#ifndef __STRING_H
#define __STRING_H

#include <sys/types.h>

int strlen(const void *s);
void reverse(char *s);
void itoa(int n, char s[]);
int memset(void *dest, unsigned int val, int n);
int memcpy(void *dest, const void *src, int n);
int memcpy_r(void *dest, const void *src, int n);
int strcpy(void *dest, const void *src);
int strcmp(const void *s1, const void *s2);
int atoi(const char *s);
char *strrchr(const char *s, int c);
extern char *strerror(int err);
extern void perror(const char *__s);
size_t strlcat(char *__dest, const char *__src, size_t __maxlen);
size_t strlcpy(char *__dest, const char *__src, size_t __maxlen);


#endif
