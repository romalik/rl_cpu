/*
 * vfs.c
 *
 * vfs.c -- Defines the interface for and structures relating to the virtual file
 * system. Written for JamesM's kernel development tutorials.
 */
#include <vfs.h>


FILE openFilesTable[OFT_SIZE];
struct f_ops f_ops_tab[DEV_SIZE] = {
    {tty_read, tty_write, tty_open, tty_close, NULL, NULL},
    {rlfs3_read, rlfs3_write, rlfs3_open, rlfs3_close, rlfs3_readdir, rlfs3_finddir}
};


struct vfs_node *vfs_root = NULL; /* The root of the filesystem. */

void vfs_init() {
    unsigned int i = 0;
    for(i = 0; i<OFT_SIZE; i++) {
        openFilesTable[i].node.device = 0;
    }
}
    
    
unsigned int
vfs_read(struct vfs_node *node, off_t offset, size_t size, unsigned int *buf)
{

	/* Has the node got a read callback? */
	if (f_ops_tab[node->device&0xff].read != NULL)
		return f_ops_tab[node->device&0xff].read(node, offset, size, buf);
	else
		return 0;
}

unsigned int
vfs_write(struct vfs_node *node, off_t offset, size_t size, unsigned int *buf)
{
	if (f_ops_tab[node->device&0xff].write != NULL)
		return f_ops_tab[node->device&0xff].write(node, offset, size, buf);
	else
		return 0;

}

FILE * 
vfs_open(unsigned int *name, int mode)
{
    //check for device here

	if (f_ops_tab[node->device&0xff].open != NULL)
		return f_ops_tab[node->device&0xff].open(name, mode);
	else
		return 0;

}

void
vfs_close(FILE * fd)
{

	if (f_ops_tab[node->device&0xff].read != NULL)
		return f_ops_tab[node->device&0xff].read(node, offset, size, buf);
	else
		return 0;
}

struct dirent *
vfs_readdir(struct vfs_node *node, unsigned int index)
{

	if (VFS_ISDIR(node) && node->readdir != NULL)
		return node->readdir(node, index);
	else
		return NULL;
}

struct vfs_node *
vfs_finddir(struct vfs_node *node, unsigned int *name)
{

	if (VFS_ISDIR(node) && node->finddir != NULL)
		return node->finddir(node, name);
	else
		return NULL;
}

