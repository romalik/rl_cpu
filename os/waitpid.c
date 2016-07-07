#include <syscall.h>

extern void syscall(void *p);

int waitpid(unsigned int pid) {
    struct waitpidSyscall s;
    s.id = __NR_waitpid;
    s.pid = pid;
    syscall(&s);
    return s.pid;
}
