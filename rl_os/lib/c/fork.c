#include <syscall.h>
#include <unistd.h>
#include <sys/types.h>

pid_t fork(void) {
    struct forkSyscall s;
    s.id = __NR_fork;
    s.pid = 0;
    syscall(&s);
    return s.pid;
}
