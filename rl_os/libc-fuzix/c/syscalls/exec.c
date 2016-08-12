#include <syscall.h>
#include <unistd.h>
#include <sys/types.h>

int execve(const char * filename, char * const argv[], char * const envp[]) {
    struct execSyscall s;
    s.id = __NR_execve;
    s.filename = filename;
    s.argv = (void *)argv;
    s.envp = (void *)envp;
    syscall(&s);
    return -1;
}
