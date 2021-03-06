#ifndef RLFS3_H__
#define RLFS3_H__

#if STANDALONE
#include <stdio.h>
#else
#include <kstdio.h>
#endif
#include <string.h>
#include <types.h>
#include <set.h>
//#include <waitq.h>
//#include <sched.h>
#define MAX_FILES 10
#define MAX_DEVS 10
#define MAX_MOUNT_POINTS 5


#define O_RDONLY        0
#define O_WRONLY        1
#define O_RDWR          2
#define O_ACCMODE       3

#define O_APPEND        4
#define O_SYNC          8
#define O_NDELAY        16
#define O_CREAT         256
#define O_EXCL          512
#define O_TRUNC         1024
#define O_NOCTTY        2048
#define O_CLOEXEC       4096
#define O_SYMLINK       8192    /* Not supported in kernel yet */

#define FD_CLOEXEC      O_CLOEXEC

#define O_BINARY        0       /* not used in Fuzix */

#define O_NONBLOCK      O_NDELAY

#define F_GETFL         0
#define F_SETFL         1
#define F_GETFD         2
#define F_SETFD         3
#define F_DUPFD         4
/* Not current implemented in Fuzix */
#define F_GETLK         5
#define F_SETLK         6
#define F_SETLKW        7

#define FNDELAY         O_NDELAY

#ifndef SEEK_SET
#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2
#endif



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
#define DIRECT_INDEXES_CNT 150
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

typedef struct fs_node { unsigned int dev; blk_t idx; } fs_node_t;


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
    int refcnt;
} FILE;

struct devOpTable {
	unsigned int registered;
    unsigned int (*write)(unsigned int minor, const unsigned int * buf, size_t n);
    unsigned int (*read)(unsigned int minor, unsigned int * buf, size_t n);
    unsigned int (*open)(unsigned int minor, FILE * file);
    unsigned int (*close)(unsigned int minor, FILE * file);
    unsigned int (*ioctl)(unsigned int minor, unsigned int request, unsigned int * buf, size_t * sz, FILE * file);
};


struct extDevOpTable {
	unsigned int registered;
	struct Process * driver;
	pid_t driverPid;
	size_t queue_user;
};


struct mtab_entry {
  fs_node_t device_node;
  fs_node_t mount_point;
  fs_node_t fs_root;
};

extern struct set mtab;

void k_regExternalDeviceCallback(struct Process * p, unsigned int major, unsigned int queue_user, unsigned int type);

unsigned int extDevCallback(struct RPCParams * params, unsigned int minor, const unsigned int * buf, size_t n, FILE * f, unsigned int req);


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
            off_t   st_size;    /* file size */
            unsigned int   st_atime;   /* last access time */
            unsigned int   st_mtime;   /* last modification time */
            unsigned int   st_ctime;   /* file creation time */
};


extern int blockRequest;



extern struct set openFiles;

extern struct fs_node fs_root;
extern struct devOpTable devList[MAX_DEVS];

void fs_mkfs(unsigned int device);

int fs_create(fs_node_t *where, const unsigned int *name, unsigned int flags,
              fs_node_t *res);

int fs_finddir(fs_node_t *where, const unsigned int *what, fs_node_t *res);
int fs_readdir(fs_node_t *dir, off_t n, dirent_t *res);

unsigned int fs_read(fs_node_t *node, off_t offset, size_t size,
                     unsigned int *buf);
unsigned int fs_write(fs_node_t *node, off_t offset, size_t size,
                      const unsigned int *buf);

FILE *fs_open(fs_node_t *node, unsigned int mode);

void fs_reset(fs_node_t *node);

size_t k_write(FILE *fd, const unsigned int *buf, size_t size);
size_t k_read(FILE *fd, unsigned int *buf, size_t size);

FILE *k_open(const void *name, unsigned int mode);
int k_stat(const void *name, struct stat * st);

void k_close(FILE *fd);
void k_seek(FILE *fd, off_t pos);

int k_isEOF(FILE *fd);

void fs_init();

FILE *k_opendir(const void *dirname);
dirent_t k_readdir(FILE *dir);

int k_mkdir(const void *__path);
int k_mkfifo(const void *__path);
int k_mknod(const void *__path, int type, unsigned int major, unsigned int minor);

int k_regDevice(unsigned int major, void *writeFunc, void *readFunc, void *openFunc, void *closeFunc, void *ioctlFunc);

int k_unlink(const char * name);

int k_ioctl(FILE * fd, int request, unsigned int * buf, size_t * sz);

size_t try_k_read(FILE *fd, unsigned int *buf, size_t size, pid_t caller, size_t scallStruct);
size_t try_k_write(FILE *fd, unsigned int *buf, size_t size, pid_t caller, size_t scallStruct);

int k_mount(const char * device_file, const char * mount_point);
int k_umount(const char * device_file);

void k_dump_mtab();

#endif
