#include <syscall.h>
#include <unistd.h>
#include <sys/types.h>


int chown(const char *path, uid_t owner, gid_t group) {
  return 0;
}

int chmod(const char *path, mode_t mode) {
  return 0;
}

int close(int fd) {
    struct closeSyscall s;
    s.id = __NR_close;
    s.fd = fd;
    syscall(&s);
    return s.fd;
}

unsigned int execve(const char *filename, void *argv[], void *envp[]) {
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

int open(const char *filename, int mode) {
    struct openSyscall s;
    s.id = __NR_open;
    s.filename = filename;
    s.mode = mode;
    syscall(&s);
    return s.mode;
}

int chdir(const char *path) {
    struct chdirSyscall s;
    s.id = __NR_chdir;
    s.path = path;

    syscall(&s);
    return s.res;
}

int mkdir(const char *path) {
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
    printf("Waitpid called!\n");
    s.id = __NR_waitpid;
    s.pid = pid;
    syscall(&s);
    return s.pid;
}

int write(int fd, const void *buf, int count) {
    struct writeSyscall s;
    s.id = __NR_write;
    s.fd = fd;
    s.size = count;
    s.buf = buf;
    syscall(&s);
    return s.size;
}
