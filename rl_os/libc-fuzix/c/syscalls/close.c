#include <syscall.h>
#include <unistd.h>
#include <sys/types.h>

int close(int fd) {
    struct closeSyscall s;
    s.id = __NR_close;
    s.fd = fd;
    syscall(&s);
    return s.fd;
}
