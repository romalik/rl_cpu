#ifndef EXECVE_H__
#define EXECVE_H__
#include <sched.h>
#include <mm.h>
#include <types.h>

size_t parseArgs(const char **argv_in, const char **envp_in, unsigned int *buf, size_t off);
unsigned int do_execve(struct Process * p, const char * filename, const char ** argv, const char ** envp);


#endif
