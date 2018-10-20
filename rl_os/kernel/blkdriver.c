#include <blkdriver.h>
#include <types.h>
#include <string.h>

static struct iblkdriver * blkdrivers[MAX_BLK_DRIVERS];

struct iblkdriver * getBlkDriver(unsigned int device) {
  size_t maj;
  maj = DEV_MAJOR(device);
  if(!blkdrivers[maj]) {
    panic("getBlkDriver: driver not registered");
  }
  return blkdrivers[maj];
}

void blkdrivers_init() {
  memset((unsigned int *)blkdrivers, 0, MAX_BLK_DRIVERS);
}

void regBlkDriver(unsigned int device, struct iblkdriver * driver) {
  size_t maj;
  maj = DEV_MAJOR(device);
  if(maj >= MAX_BLK_DRIVERS) {
    panic("regBlkDriver: out of bounds");
  }
  if(blkdrivers[maj]) {
    panic("regBlkDriver: double register");
  }
  blkdrivers[maj] = driver;
}

