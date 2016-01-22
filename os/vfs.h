#ifndef VFS_H
#define VFS_H
/*
 * vfs.h -- VFS stuff.
 *
 * Written for JamesM's kernel development tutorials.
 */

#include <types.h>

#include <tty.h>
#include <rlfs3.h>

// max open files
#define OFT_SIZE 15
#define DEV_SIZE 5

#define VFS_FREE_ENTRY 0x00
#define VFS_FILE 0x01
#define VFS_DIRECTORY 0x02
#define VFS_CHARDEVICE 0x03
#define VFS_BLOCKDEVICE 0x04
#define VFS_PIPE 0x05
#define VFS_SYMLINK 0x06
#define VFS_MOUNTPOINT 0x08 /* Is the file an active mountpoint? */
/* Notice that VFS_MOUNTPOINT is given the value 8, not 7. This is so that it
   can
   be bitwise-OR'd in with VFS_DIRECTORY */
#define VFS_ISDIR(node) (((node)->flags & 0x7) == VFS_DIRECTORY)

struct vfs_node;

typedef struct __FILE {
    struct vfs_node node;
    off_t size;
    off_t pos;
} FILE;

typedef unsigned int (*vfs_read_func)(struct vfs_node *node, off_t offset,
                                      size_t size, char *buf);
typedef unsigned int (*vfs_write_func)(struct vfs_node *node, off_t offset,
                                       size_t size, char *buf);
typedef unsigned int (*vfs_open_func)(unsigned int *name, int mode);
typedef void (*vfs_close_func)(FILE *fd);
typedef struct dirent *(*vfs_readdir_func)(struct vfs_node *node,
                                           unsigned int index);
typedef struct vfs_node *(*vfs_finddir_func)(struct vfs_node *node, char *name);

struct vfs_node {
    unsigned int device;
    blk_t inode;
};

struct f_ops {
    vfs_read_func read;
    vfs_write_func write;
    vfs_open_func open;
    vfs_close_func close;
    vfs_readdir_func readdir;
    vfs_finddir_func finddir;
};

struct dirent {
    char name[15]; /* Filename. */
    blk_t inode;   /* Inode number. Required by POSIX. */
    unsigned int device;
};

extern FILE openFilesTable[];
extern f_ops f_ops_tab[];

extern struct vfs_node *vfs_root; /* The root of the filesystem. */

unsigned int vfs_read(struct vfs_node *node, off_t offset, size_t size,
                      unsigned int *buf);
unsigned int vfs_write(struct vfs_node *node, off_t offset, size_t size,
                       unsigned int *buf);

FILE *vfs_open(unsigned int *name int mode);

void vfs_close(FILE *fd);
struct dirent *vfs_readdir(struct vfs_node *node, unsigned int index);
struct vfs_node *vfs_finddir(struct vfs_node *node, unsigned int *name);

#endif /* ndef VFS_H */
