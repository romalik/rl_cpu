#include <syscall.h>
#include <open.h>

int open(unsigned int *filename, int mode) {
    struct openSyscall s;
    s.id = __NR_open;
    s.filename = filename;
    s.mode = mode;
    syscall(&s);
    return s.mode;
}
