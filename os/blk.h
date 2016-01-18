#ifndef BLK_H__
#define BLK_H__

#include <types.h>

#define BLOCK_FREE      0x00
#define BLOCK_USED      0x01
#define BLOCK_MODIFIED  0x02
#define BLOCK_LOCKED    0x04

#define BLOCK_CACHE_SIZE 16

struct Block {
  unsigned int flags;
  unsigned int device;
  unsigned int n;
  unsigned int cnt;
  unsigned int *data;
};

extern struct Block blockCache[];

struct Block * bread(unsigned int device, unsigned int n);
void bfree(struct Block * b);
void block_init();


#endif
