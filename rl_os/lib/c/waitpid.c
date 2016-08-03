#include <syscall.h>
#include <unistd.h>
#include <sys/types.h>

int waitpid(unsigned int pid) {
    struct waitpidSyscall s;
    printf("Waitpid called!\n");
    s.id = __NR_waitpid;
    s.pid = pid;
    syscall(&s);
    return s.pid;
}
