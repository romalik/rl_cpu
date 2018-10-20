#ifndef BLKDRIVER_H__
#define BLKDRIVER_H__
#include <blk.h>
#include <types.h>

#define MAX_BLK_DRIVERS 3

struct iblkdriver {
  void (*init)(unsigned int device);
  size_t (*write)(unsigned int device, unsigned int blk, unsigned int * buffer);
  size_t (*read)(unsigned int device, unsigned int blk, unsigned int * buffer);
  void (*free)(unsigned int device);
};

void blkdrivers_init();
struct iblkdriver * getBlkDriver(unsigned int device);
void regBlkDriver(unsigned int device, struct iblkdriver * driver);


#endif
