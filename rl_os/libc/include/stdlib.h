#ifndef __STDLIB_H
#define __STDLIB_H
#include <types.h>
#include <malloc.h>
#include <syscall.h>

/* Don't overwrite user definitions of NULL */
#ifndef NULL
#define NULL ((void *) 0)
#endif

/* Returned by `div' */
typedef struct {
	int	quot;		/* Quotient */
	int	rem;		/* Remainder */
} div_t;

/* Returned by `ldiv' */
typedef struct {
	long int quot;		/* Quotient */
	long int rem;		/* Remainder */
} ldiv_t;

/* For program termination */
#define EXIT_FAILURE 1
#define EXIT_SUCCESS 0

void exit(int __status);
void abort(void);


int rand(void);
void srand(unsigned int __seed);

/* FIXME: the *toa formats are not standard so belong in a fuzix namespace */
/* FIXME: untangle all our excessive ultoa etc variants */
char *ultoa(unsigned long __value, char *__strP, int __radix);
char *ltoa(long __value, char *__strP, int __radix);

int atoi(const char *__nptr);
long atol(const char *__nptr);
double atof(const char *__nptr);

int abs(int __i);
long labs(long __i);
char *_itoa(int __value);
char *_uitoa(unsigned int __value);
char *_ltoa(long __value);
char *_ultoa(unsigned long __value);

char *__ultostr(unsigned long value, int __radix);
char *__ltostr(long __value, int __radix);


long strtol(const char *__nptr, char **__endptr, int __base);
unsigned long strtoul(const char *__nptr,
				   char **__endptr, int __base);

int mkstemp(char *__template);
int mkstemps(char *__template, int __suffix);

#ifndef __HAS_NO_DOUBLES__
double strtod(const char *__nptr, char **__endptr);
#endif

extern char **environ;
char *getenv(char *__name);
int putenv(char *__name);
int setenv(char *__name, char *__value, int __overwrite);
void unsetenv(char *__name);

/* FIXME: atexit_t doesn't appear to be a standard type so should __atexit_t ?? */
typedef void (*atexit_t)(void);
int atexit(atexit_t __function);

char *crypt(char *__key, char *__salt);

typedef int (*cmp_func_t)(const void *__a, const void *__b);

int _bsearch;
void *bsearch(void *__key, void *__base, size_t __num, size_t __size, cmp_func_t __cmp);
void *lfind(void *__key, void *__base, size_t *__num, size_t __size, cmp_func_t __cmp);
void *lsearch(void *__key, void *__base, size_t *__num, size_t __size, cmp_func_t __cmp);
void *_qbuf;
void qsort(void *__base, size_t __num, size_t __size, cmp_func_t __cmp);

#define mb_len(a,b)	strnlen(a,b)

int opterr;
int optind;
char *optarg;
int getopt(int __argc, char *__argv[], char *__optstring);

char *getpass(char *__prompt);

/*
int _argc;
char **_argv;
*/


int getloadavg(unsigned int __loadavg[], int __nelem);

double drand48(void);
double erand48(unsigned short __xsubi[3]);
unsigned long jrand48(unsigned short __xsubi[3]);
void lcong48(unsigned short __param[7]);
long lrand48(void);
unsigned long mrand48(void);
long nrand48(unsigned short __xsubi[3]);
unsigned short *seed48(unsigned short __seed16v[3]);
void srand48(long __sedval);

#endif /* __STDLIB_H */
