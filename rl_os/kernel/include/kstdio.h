#ifndef __KSTDIO_H
#define __KSTDIO_H

#include "string.h"

void outb(int port, int value);
int inb(int port);


int putc(char c);
int puts(char *s);
void putDigit(int a);
void putNum(int a);
int printf(const char *format, ...);
int sprintf(char *out, const char *format, ...);
int getc();
unsigned int gettime();

#endif
