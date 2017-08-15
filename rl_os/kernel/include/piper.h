#ifndef PIPER_H__
#define PIPER_H__

#define PIPE_MAX_LENGTH 256
#define MAX_PIPES 10

#include <types.h>

unsigned int piper_read(unsigned int minor, unsigned int * buf, size_t n);
unsigned int piper_write(unsigned int minor, const unsigned int * buf, size_t n);
unsigned int piper_close(unsigned int minor);
void piper_init();
#endif
