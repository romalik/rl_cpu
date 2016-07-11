#ifndef TTY_H__
#define TTY_H__

#include <memmap.h>

unsigned int tty_read(unsigned int minor);
unsigned int tty_write(unsigned int minor, unsigned int val);

#endif
