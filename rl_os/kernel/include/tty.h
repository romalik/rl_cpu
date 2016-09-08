#ifndef TTY_H__
#define TTY_H__

#include <memmap.h>
#include <types.h>

unsigned int tty_read(unsigned int minor, unsigned int * buf, size_t n);
unsigned int tty_write(unsigned int minor, const unsigned int * buf, size_t n);

#endif
