#include <syscall.h>
#include <unistd.h>
#include <sys/types.h>

int chown(const char *path, uid_t owner, gid_t group) {
    return 0;
}

int chmod(const char *path, mode_t mode) {
    return 0;
}
