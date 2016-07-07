#ifndef __STDIO_H
#define __STDIO_H

#include "string.h"

int putc(char c);
int puts(char *s);
void putDigit(int a);
void putNum(int a);
int printf(const char *format, ...);
int sprintf(char *out, const char *format, ...);
int getc();
// int kgetc();
// int kputc(char c);
unsigned int gettime();

#define stdin 0
#define stdout 1
#define stderr 2

#endif
