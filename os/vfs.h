#ifndef VFS_H
#define VFS_H
/*
 * vfs.h -- VFS stuff.
 *
 * Written for JamesM's kernel development tutorials.
 */

#include <types.h>


#define MAX_INODE_CACHE 32

#define VFS_FREE_ENTRY  0x00
#define VFS_FILE        0x01
#define VFS_DIRECTORY   0x02
#define VFS_CHARDEVICE  0x03
#define VFS_BLOCKDEVICE 0x04
#define VFS_PIPE        0x05
#define VFS_SYMLINK     0x06
#define VFS_MOUNTPOINT  0x08 /* Is the file an active mountpoint? */
/* Notice that VFS_MOUNTPOINT is given the value 8, not 7. This is so that it can
   be bitwise-OR'd in with VFS_DIRECTORY */
#define	VFS_ISDIR(node)	(((node)->flags & 0x7) == VFS_DIRECTORY)


struct vfs_node;

typedef unsigned int (*vfs_read_func)(struct vfs_node *node, unsigned int offset, unsigned int size, char *buf);
typedef unsigned int (*vfs_write_func)(struct vfs_node *node, unsigned int offset, unsigned int size, char *buf);
typedef unsigned int (*vfs_open_func)(struct vfs_node *node, int mode);
typedef void (*vfs_close_func)(struct vfs_node *node);
typedef struct dirent * (*vfs_readdir_func)(struct vfs_node *node, unsigned int index);
typedef struct vfs_node * (*vfs_finddir_func)(struct vfs_node *node, char *name);


struct vfs_node {
	unsigned int flags;       /* Includes the node type. See #defines above. */
	unsigned int inode;       /* This is device-specific - provides a way for a filesystem to identify files. */
	unsigned int size;      /* Size of the file, in bytes. */
	vfs_read_func read;
	vfs_write_func write;
	vfs_open_func open;
	vfs_close_func close;
	vfs_readdir_func readdir;
	vfs_finddir_func finddir;
	//struct vfs_node *ptr; /* Used by mountpoints and symlinks. */
};

struct dirent /* One of these is returned by the readdir call, according to POSIX. */
{
	char name[15]; /* Filename. */
	unsigned int inode;     /* Inode number. Required by POSIX. */
};


extern struct vfs_node *vfs_root; /* The root of the filesystem. */


unsigned int vfs_read(struct vfs_node *node, unsigned int offset, unsigned int size, char *buf);
unsigned int vfs_write(struct vfs_node *node, unsigned int offset, unsigned int size, char *buf);
unsigned int vfs_open(struct vfs_node *node, int mode);
void vfs_close(struct vfs_node *node);
struct dirent * vfs_readdir(struct vfs_node *node, unsigned int index);
struct vfs_node * vfs_finddir(struct vfs_node *node, char *name);


extern struct vfs_node nodeCache[MAX_INODE_CACHE];

struct vfs_node * getFreeNodeEntry();
void vfs_init();

#endif /* ndef VFS_H */
