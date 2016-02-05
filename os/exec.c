#include <syscall.h>
#include <exec.h>

unsigned int execve(unsigned int *filename, unsigned int *argv[],
                    unsigned int *envp[]) {
    struct execSyscall s;
    s.id = __NR_execve;
    s.filename = filename;
    s.argv = argv;
    s.envp = envp;
    syscall(&s);
    return -1;
}
