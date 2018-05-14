#ifndef TTY_H__
#define TTY_H__

#include <memmap.h>
#include <types.h>
#include <rlfs3.h>

unsigned int tty_open(unsigned int minor, FILE * fd);
unsigned int tty_close(unsigned int minor, FILE * fd);

unsigned int tty_read(unsigned int minor, unsigned int * buf, size_t n);
unsigned int tty_write(unsigned int minor, const unsigned int * buf, size_t n);

#endif
