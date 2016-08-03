#include <syscall.h>
#include <unistd.h>
#include <sys/types.h>

int open(const char *filename, int flags, ...) {
    struct openSyscall s;
    s.id = __NR_open;
    s.filename = filename;
    s.mode = flags;
    syscall(&s);
    return s.mode;
}
