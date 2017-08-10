#ifndef __STRING_H
#define __STRING_H
#include <types.h>
#include <stddef.h>

/* Basic string functions */
size_t strlen(const char *__s);

char *strcat(char *__dest, const char *__src);
char *strcpy(char *__dest, const char *__src);
int strcmp(const char *__s1, const char *__s2);

char *strncat(char *__dest, const char *__src, size_t __n);
char *strncpy(char *__dest, const char *__src, size_t __n);
int strncmp(const char *__s1, const char *__s2, size_t __n);

int stricmp(const char *__s1, const char *__s2);
int strnicmp(const char *__s1, const char *__s2, size_t __n);

int strcasecmp(const char *__s1, const char *__s2);
int strncasecmp(const char *__s1, const char *__s2, size_t __n);

char *strchr(const char *__s, int __c);
char *strrchr(const char *__s , int __c);
char *strdup(const char *__s);
/* FIXME: missing but in POSIX */
//char *strndup(const char *__s, int __n);

/* Basic mem functions */
void *memcpy(void *__dest, const void *__src, size_t __n);
void *memccpy(void *__dest, const void *__src, int __c, size_t __n);
void *memchr(const void *__src, int __c, size_t __n);
void *memset(void *__s, int __c, size_t __n);
int memcmp(const void *__s1, const void *__s2, size_t __n);

void *memmove(void *__dest, const void *__src, size_t __n);

/* BSDisms */
char *index(const char *__s, int __c);
char *rindex(const char *__s, int __c);
void bcopy(const void *__src, void *__dst, size_t __n);
void bzero(void *__dst, size_t __n);

/* Other common BSD functions */
char *strpbrk(const char *__s, const char *__accept);
char *strsep(char **__stringp, const char *__delim);
char *strstr(const char *__haystack, const char *__needle);
char *strtok(char *__str, const char *__delim);
size_t strcspn(const char *__s, const char *__reject);
size_t strspn(const char *__s, const char *__accept);

size_t strlcpy(char *__dest, const char *__src, size_t __maxlen);
size_t strlcat(char *__dest, const char *__src, size_t __maxlen);

/* FIXME: GNUism */
char *strcasestr(const char *__needle, const char *__haystack);

/* Later ISOisms */
size_t strnlen(const char *__s, size_t __maxlen);
size_t strxfrm(char *__dest, const char *__src, size_t __n);
int strcoll(const char *__s1, const char *__s2);

const char *strsignal(int __sig);
char *strerror(int __errno);


#endif		/* END OF DEFINITION	STRING.H */
