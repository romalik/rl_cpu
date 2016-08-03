#include <syscall.h>
#include <unistd.h>
#include <sys/types.h>

int chdir(const char *path) {
    struct chdirSyscall s;
    s.id = __NR_chdir;
    s.path = path;

    syscall(&s);
    return s.res;
}
