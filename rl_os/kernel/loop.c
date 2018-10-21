#include <loop.h>
#include <rlfs3.h>
#include <blkdriver.h>

#define MAX_LOOP_DEVS 3

static struct iblkdriver loop_driver;

struct loop_device_entry {
  fs_node_t device_node;
  fs_node_t virtual_device_file;
};

struct loop_device_entry loop_devs[MAX_LOOP_DEVS];


//hack
extern int fs_lookup(const unsigned int *name, fs_node_t *parent, fs_node_t *res);


int loop_ioctl(unsigned int minor, unsigned int request, unsigned int * buf, size_t * sz, FILE * file) {
  int rv;
  printf("loop_ioctl: buf = %s\n", buf);
  *sz = 0;
  if(request == 0) {
    if(minor > MAX_LOOP_DEVS) {
      panic("loop_ioctl: out of bounds");
    }
    loop_devs[minor].device_node = file->node;

    rv = fs_lookup(buf, NULL, &loop_devs[minor].virtual_device_file);
    // printf("File lookup result %d\n", rv);
    if (rv != FS_OK) {
      printf("loop_ioctl: lookup fail\n");
    }

  }
}

void loop_init(unsigned int device) {

}

unsigned int loop_read_block(unsigned int device, unsigned int block, unsigned int *Buffer) {
  unsigned int minor = DEV_MINOR(device);
  return fs_read(&loop_devs[minor].virtual_device_file, block*256, 256, Buffer);
}
unsigned int loop_write_block(unsigned int device, unsigned int block, unsigned int *Buffer) {
  unsigned int minor = DEV_MINOR(device);
  return fs_write(&loop_devs[minor].virtual_device_file, block*256, 256, Buffer);
}

struct iblkdriver * loop_get_driver() {
  loop_driver.init = &loop_init;
  loop_driver.write = &loop_write_block;
  loop_driver.read = &loop_read_block;
  loop_driver.free = 0;
  return &loop_driver;
}

