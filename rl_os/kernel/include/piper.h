#ifndef PIPER_H__
#define PIPER_H__

#define PIPE_MAX_LENGTH 256
#define MAX_PIPES 3

#include <types.h>
#include <rlfs3.h>

unsigned int piper_open(unsigned int minor, FILE * fd);
unsigned int piper_read(unsigned int minor, unsigned int * buf, size_t n);
unsigned int piper_write(unsigned int minor, const unsigned int * buf, size_t n);
unsigned int piper_close(unsigned int minor, FILE * fd);
void piper_init();
int piper_getFreePipe();
#endif
