#include <syscall.h>
#include <unistd.h>

int close(int fd) {
    struct closeSyscall s;
    s.id = __NR_close;
    s.fd = fd;
    syscall(&s);
    return s.fd;
}

unsigned int execve(void *filename, void *argv[], void *envp[]) {
    struct execSyscall s;
    s.id = __NR_execve;
    s.filename = filename;
    s.argv = argv;
    s.envp = envp;
    syscall(&s);
    return -1;
}

void exit(int code) {
    struct exitSyscall s;
    s.id = __NR_exit;
    s.code = code;
    syscall(&s);
}

unsigned int fork() {
    struct forkSyscall s;
    s.id = __NR_fork;
    s.pid = 0;
    syscall(&s);
    return s.pid;
}

int open(void *filename, int mode) {
    struct openSyscall s;
    s.id = __NR_open;
    s.filename = filename;
    s.mode = mode;
    syscall(&s);
    return s.mode;
}

int chdir(void *path) {
    struct chdirSyscall s;
    s.id = __NR_chdir;
    s.path = path;
    
    syscall(&s);
    return s.res;
}

int mkdir(void *path) {
    struct mkdirSyscall s;
    s.id = __NR_mkdir;
    s.path = path;
    
    syscall(&s);
    return s.res;
}

int read(int fd, void *buf, int count) {
    struct readSyscall s;
    s.id = __NR_read;
    s.fd = fd;
    s.size = count;
    s.buf = buf;
    syscall(&s);
    return s.size;
}

int waitpid(unsigned int pid) {
    struct waitpidSyscall s;
    s.id = __NR_waitpid;
    s.pid = pid;
    syscall(&s);
    return s.pid;
}

int write(int fd, void *buf, int count) {
    struct writeSyscall s;
    s.id = __NR_write;
    s.fd = fd;
    s.size = count;
    s.buf = buf;
    syscall(&s);
    return s.size;
}
