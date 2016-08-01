#ifndef RLFS3_H__
#define RLFS3_H__

#if STANDALONE
#include <stdio.h>
#else
#include <kstdio.h>
#endif
#include <string.h>
#include <types.h>

#define MAX_FILES 10
#define MAX_DEVS 10

/*
#define FS_NONE 0
#define FS_FILE 1
#define FS_DIR 2
#define FS_CHAR_DEV 3
#define FS_BLOCK_DEV 4
#define FS_PIPE 5
#define FS_LINK 6
*/

#define FS_MODE_NONE 0
#define FS_MODE_READ 'r'
#define FS_MODE_WRITE 'w'
#define FS_MODE_APPEND 'a'

#define FS_OK 0
#define FS_NO_FILE 1
#define FS_NOT_A_DIR 2
#define FS_FILE_EXISTS 3
#define FS_NO_DIR 4

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

struct devOpTable {
    unsigned int (*write)(unsigned int minor, unsigned int val);
    unsigned int (*read)(unsigned int minor);
};

typedef struct dirent {
    unsigned int name[31];
    blk_t idx;
} dirent_t;

typedef struct __FILE {
    unsigned int mode;
    fs_node_t node;
    off_t size;
    off_t pos;
    unsigned int device; // for dev files
    unsigned int flags;
} FILE;
/*
typedef struct __stat {
    fs_node_t node;
    off_t size;
    unsigned int flags;
} stat_t;
*/
#define S_IFMT      0170000
#define S_IFSOCK    0140000     /* Reserved, not used */
#define S_IFLNK     0120000     /* Reserved, not used */
#define S_IFREG     0100000
#define S_IFBLK     0060000
#define S_IFDIR     0040000
#define S_IFCHR     0020000
#define S_IFIFO     0010000

#define S_ISUID     0004000
#define S_ISGID     0002000
#define S_ISVTX     0001000     /* Reserved, not used */
#define S_IRWXU     0000700
#define S_IRUSR     0000400
#define S_IWUSR     0000200
#define S_IXUSR     0000100
#define S_IRWXG     0000070
#define S_IRGRP     0000040
#define S_IWGRP     0000020
#define S_IXGRP     0000010
#define S_IRWXO     0000007
#define S_IROTH     0000004
#define S_IWOTH     0000002
#define S_IXOTH     0000001

#define S_ISREG(m)  (((m) & S_IFMT) == S_IFREG)
#define S_ISDIR(m)  (((m) & S_IFMT) == S_IFDIR)
#define S_ISCHR(m)  (((m) & S_IFMT) == S_IFCHR)
#define S_ISBLK(m)  (((m) & S_IFMT) == S_IFBLK)
#define S_ISFIFO(m) (((m) & S_IFMT) == S_IFIFO)
#define S_ISLNK(m)  (((m) & S_IFMT) == S_IFLNK)
#define S_ISSOCK(m) (((m) & S_IFMT) == S_IFSOCK)

#define S_ISDEV(m)  (((m) & S_IFCHR) == S_IFCHR)

struct stat {
            unsigned int   st_dev;     /* device number */
            unsigned int   st_ino;     /* inode number */
            unsigned int   st_mode;    /* file mode */
            unsigned int   st_nlink;   /* number of links */
            unsigned int   st_uid;     /* owner id */
            unsigned int   st_gid;     /* owner group */
            unsigned int   st_rdev;    /* */
            unsigned int   st_size;    /* file size */
            unsigned int   st_atime;   /* last access time */
            unsigned int   st_mtime;   /* last modification time */
            unsigned int   st_ctime;   /* file creation time */
};



extern FILE openFiles[MAX_FILES];
extern struct fs_node fs_root;
extern struct devOpTable devList[MAX_DEVS];

void fs_mkfs();

int fs_create(fs_node_t *where, unsigned int *name, unsigned int flags,
              fs_node_t *res);

int fs_finddir(fs_node_t *where, unsigned int *what, fs_node_t *res);
int fs_readdir(fs_node_t *dir, off_t n, dirent_t *res);

unsigned int fs_read(fs_node_t *node, off_t offset, size_t size,
                     unsigned int *buf);
unsigned int fs_write(fs_node_t *node, off_t offset, size_t size,
                      unsigned int *buf);

FILE *fs_open(fs_node_t *node, unsigned int mode);
void fs_close(FILE *fd);

size_t k_write(FILE *fd, unsigned int *buf, size_t size);
size_t k_read(FILE *fd, unsigned int *buf, size_t size);

FILE *k_open(void *name, unsigned int mode);
struct stat k_stat(void *name);

void k_close(FILE *fd);
void k_seek(FILE *fd, off_t pos);

int k_isEOF(FILE *fd);

void fs_init();

FILE *k_opendir(void *dirname);
dirent_t k_readdir(FILE *dir);

int k_mkdir(void *__path);
int k_mknod(void *__path, int type, unsigned int major, unsigned int minor);

int k_regDevice(unsigned int major, void *writeFunc, void *readFunc);

#endif
