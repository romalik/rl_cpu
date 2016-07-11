#ifndef UNISTD_H__
#define UNISTD_H__
#include <syscall.h>
int close(int fd);
unsigned int execve(void *filename, void *argv[], void *envp[]);
void exit(int code);
unsigned int fork();
int open(void *filename, int mode);
int read(int fd, void *buf, int count);
int waitpid(unsigned int pid);
int write(int fd, void *buf, int count);
int chdir(void * path);
int mkdir(void * path);
#endif
