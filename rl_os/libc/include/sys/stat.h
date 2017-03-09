#ifndef STAT_H__
#define STAT_H__
#include <syscall.h>
#include <sys/types.h>

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
        unsigned long   st_atime;   /* last access time */
        unsigned long   st_mtime;   /* last modification time */
        unsigned long   st_ctime;   /* file creation time */
};


int stat(const char * name, struct stat * buf);
int chmod(const char *path, mode_t mode);
int creat(const char *pathname, mode_t mode);


#endif
