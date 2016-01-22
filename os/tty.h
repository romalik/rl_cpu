#ifndef TTY_H__
#define TTY_H__

#include <vfs.h>
#include <memmap.h>


unsigned int tty_read(struct vfs_node *node, off_t offset, size_t size, unsigned int *buf);
unsigned int tty_write(struct vfs_node *node, off_t offset, size_t size, unsigned int *buf);
FILE * tty_open(struct



#endif
