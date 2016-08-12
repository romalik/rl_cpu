#include <syscall.h>
#include <unistd.h>
#include <sys/types.h>

pid_t waitpid(pid_t pid, int *status, int options) {
    struct waitpidSyscall s;
    printf("Waitpid called!\n");
    s.id = __NR_waitpid;
    s.pid = pid;
    syscall(&s);
    return s.pid;
}
