#include <blkdriver.h>
#include <types.h>
#include <string.h>

#define BAD_MAJOR 0xffff
struct major_driver_pair {
  unsigned int major;
  struct iblkdriver * driver;
};

static struct major_driver_pair blkdrivers[MAX_BLK_DRIVERS];

static struct major_driver_pair * getDriverEntry(unsigned int major) {
  size_t i = 0;
  for(i = 0; i<MAX_BLK_DRIVERS; i++) {
    if(blkdrivers[i].major == major) {
      return &blkdrivers[i];
    }
  }
  return 0;
}


struct iblkdriver * getBlkDriver(unsigned int device) {
  size_t maj;
  struct major_driver_pair * el;
  maj = DEV_MAJOR(device);
  if(!(el = getDriverEntry(maj))) {
    panic("getBlkDriver: driver not registered");
  }
  return el->driver;
}


void blkdrivers_init() {
  memset((unsigned int *)blkdrivers, BAD_MAJOR, MAX_BLK_DRIVERS*sizeof(struct major_driver_pair));
}

void regBlkDriver(unsigned int device, struct iblkdriver * driver) {
  size_t maj;
  struct major_driver_pair * el;
  maj = DEV_MAJOR(device);
  if(el = getDriverEntry(maj)) {
    panic("regBlkDriver: double register");
  }
  if(!(el = getDriverEntry(BAD_MAJOR))) {
    panic("regBlkDriver: overflow");
  }

  el->major = maj;
  el->driver = driver;
}

