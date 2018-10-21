#ifndef LOOP_H__
#define LOOP_H__

#include <rlfs3.h>


int loop_ioctl(unsigned int minor, unsigned int request, unsigned int * buf, size_t * sz, FILE * file);
void loop_init();

unsigned int loop_read_block(unsigned int device, unsigned int block, unsigned int *Buffer);
unsigned int loop_write_block(unsigned int device, unsigned int block, unsigned int *Buffer);

struct iblkdriver * loop_get_driver();



#endif
