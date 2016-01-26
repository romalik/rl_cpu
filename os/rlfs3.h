#ifndef RLFS3_H__
#define RLFS3_H__
#include <stdio.h>
#include <string.h>
#include "ata.h"
#include <types.h>
#define MAX_FILES 10

#define FS_NONE 0
#define FS_FILE 1
#define FS_DIR 2
#define FS_CHAR_DEV 3
#define FS_BLOCK_DEV 4
#define FS_PIPE 5
#define FS_LINK 6

#define FS_MODE_NONE 0
#define FS_MODE_READ 'r'
#define FS_MODE_WRITE 'w'
#define FS_MODE_APPEND 'a'

/* RLFS3 filesystem
 *
 * Block0: superblock (not impl)
 * Block1-33: free block bitmap
 * Block34: root node
 *
 */

/* Fs node header:
 *  Flags (16):
 *    File
 *    Dir
 *    CharDevice
 *    BlockDevice
 *    Pipe
 *    Link
 *    rwxrwxrwx
 */

/* File/dir node:
 *  Flags
 *  SizeL(16)
 *  SizeH(16)
 *  150 indexes
 *  100 index-indexes
 *  1 triple-index
 */

/* Dir contents:
 * 0: [31 words - name, node idx]
 * 32: ...total 8...
 */

/* Device/pipe node:
 *  dev/pipe id
 */

typedef struct fs_node { blk_t idx; } fs_node_t;

typedef struct dirent {
    unsigned int name[31];
    blk_t idx;
} dirent_t;

typedef struct __FILE {
    unsigned int mode;
    fs_node_t node;
    off_t size;
    off_t pos;
} FILE;

typedef struct __stat {
    off_t size;
    unsigned int flags;
} stat_t;

extern FILE openFiles[MAX_FILES];
extern struct fs_node fs_root;

void fs_mkfs();

fs_node_t fs_create(fs_node_t where, unsigned int *name, unsigned int flags);

fs_node_t fs_finddir(fs_node_t where, unsigned int *what);
dirent_t fs_readdir(fs_node_t dir, off_t n);

unsigned int fs_read(fs_node_t node, off_t offset, size_t size,
                     unsigned int *buf);
unsigned int fs_write(fs_node_t node, off_t offset, size_t size,
                      unsigned int *buf);

FILE *fs_open(fs_node_t node, unsigned int mode);
void fs_close(FILE *fd);

size_t k_write(FILE *fd, unsigned int *buf, size_t size);
size_t k_read(FILE *fd, unsigned int *buf, size_t size);

FILE *k_open(void *name, unsigned int mode);
stat_t k_stat(void *name);

void k_close(FILE *fd);
void k_seek(FILE *fd, off_t pos);

int k_isEOF(FILE *fd);

void fs_init();

FILE * k_opendir(void *dirname);
dirent_t k_readdir(FILE * dir);

int k_mkdir(void *__path);

#endif
