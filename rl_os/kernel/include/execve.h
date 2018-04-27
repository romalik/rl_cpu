#ifndef EXECVE_H__
#define EXECVE_H__
#include <sched.h>
#include <mm.h>
#include <types.h>
#include <syscall.h>

size_t parseArgs(size_t argv_in, size_t envp_in, unsigned int *buf, size_t off, struct execSyscall * s, struct Process * tp);
unsigned int do_execve(struct Process * p, struct execSyscall *s);


#endif
