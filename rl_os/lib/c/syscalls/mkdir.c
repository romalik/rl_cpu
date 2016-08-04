#include <syscall.h>
#include <unistd.h>
#include <sys/types.h>

int mkdir(const char *path) {
    struct mkdirSyscall s;
    s.id = __NR_mkdir;
    s.path = path;

    syscall(&s);
    return s.res;
}
