#include <syscall.h>
#include <unistd.h>
#include <sys/types.h>

int read(int fd, void *buf, int count) {
    struct readSyscall s;
    s.id = __NR_read;
    s.fd = fd;
    s.size = count;
    s.buf = buf;
    syscall(&s);
    return s.size;
}
