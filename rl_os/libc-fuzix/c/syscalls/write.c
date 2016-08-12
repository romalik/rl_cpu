#include <syscall.h>
#include <unistd.h>
#include <sys/types.h>

int write(int fd, const void *buf, int count) {
    struct writeSyscall s;
    s.id = __NR_write;
    s.fd = fd;
    s.size = count;
    s.buf = buf;
    syscall(&s);
    return s.size;
}
