#ifndef UNISTD_H__
#define UNISTD_H__
#include <syscall.h>
#include <sys/types.h>
int close(int fd);
unsigned int execve(const char *filename, void *argv[], void *envp[]);
void exit(int code);
unsigned int fork();
int open(const char *filename, int mode);
int read(int fd, void *buf, int count);
int waitpid(unsigned int pid);
int write(int fd, const void *buf, int count);
int chdir(const char * path);
int mkdir(const char * path);
int chmod(const char *path, mode_t mode);
int chown(const char *path, uid_t owner, gid_t group);
#endif
