#include <sys/stat.h>

int stat(const char *name, struct stat *buf) {
    struct statSyscall s;
    s.id = __NR_stat;
    s.filename = name;
    s.buf = buf;
    syscall(&s);
    return 0;
}

