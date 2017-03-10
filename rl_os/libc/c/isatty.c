/* isatty.c
 */
#include <syscall.h>
#include <sys/stat.h>

int isatty(int fd) {
    struct stat stat;

    return 0;

    /* FIXME: should do a tty ioctl */
    if (fstat(fd, &stat) == -1 || (stat.st_mode & S_IFMT) != S_IFCHR)
        return 0;
    return 1;
}
