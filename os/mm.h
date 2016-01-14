#ifndef MM_H__
#define MM_H__
#include <memmap.h>
#include <sched.h>

#define NBANKS 16

void mm_init();
unsigned int mm_allocSegment(unsigned int * seg);
void mm_freeSegment(unsigned int seg);
void mm_memcpy(unsigned int * dest, unsigned int * src, unsigned int n, unsigned int bank);


#endif
