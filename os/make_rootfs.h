//#include "types.h"

typedef uint16_t blk_t;

#define BLOCK_FREE 0x00
#define BLOCK_USED 0x01
#define BLOCK_MODIFIED 0x02
#define BLOCK_LOCKED 0x04

#define BLOCK_CACHE_SIZE 16

struct Block {
    uint16_t flags;
    uint16_t device;
    uint16_t n;
    uint16_t cnt;
    uint16_t *data;
};

extern struct Block blockCache[];

struct Block *bread(uint16_t device, uint16_t n);
void bfree(struct Block *b);
void block_init();
void block_sync();

#define MAX_FILES 10
#define MAX_DEVS 10

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
    uint16_t (*write)(uint16_t minor, uint16_t val);
    uint16_t (*read)(uint16_t minor);
};

typedef struct dirent {
    uint16_t name[31];
    blk_t idx;
} dirent_t;

typedef struct __FILE {
    uint16_t mode;
    fs_node_t node;
    off_t size;
    off_t pos;
    uint16_t device; // for dev files
    uint16_t flags;
} FILE;

typedef struct __stat {
    fs_node_t node;
    off_t size;
    uint16_t flags;
} stat_t;

extern FILE openFiles[MAX_FILES];
extern struct fs_node fs_root;
extern struct devOpTable devList[MAX_DEVS];

void fs_mkfs();

int fs_create(fs_node_t *where, uint16_t *name, uint16_t flags, fs_node_t *res);

int fs_finddir(fs_node_t *where, uint16_t *what, fs_node_t *res);
int fs_readdir(fs_node_t *dir, off_t n, dirent_t *res);

uint16_t fs_read(fs_node_t *node, off_t offset, size_t size, uint16_t *buf);
uint16_t fs_write(fs_node_t *node, off_t offset, size_t size, uint16_t *buf);

FILE *fs_open(fs_node_t *node, uint16_t mode);
void fs_close(FILE *fd);

size_t k_write(FILE *fd, uint16_t *buf, size_t size);
size_t k_read(FILE *fd, uint16_t *buf, size_t size);

FILE *k_open(void *name, uint16_t mode);
stat_t k_stat(void *name);

void k_close(FILE *fd);
void k_seek(FILE *fd, off_t pos);

int k_isEOF(FILE *fd);

void fs_init();

FILE *k_opendir(void *dirname);
dirent_t k_readdir(FILE *dir);

int k_mkdir(void *__path);
int k_mknod(void *__path, int type, uint16_t major, uint16_t minor);

int k_regDevice(uint16_t major, void *writeFunc, void *readFunc);
