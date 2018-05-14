#ifndef FORK_H__
#define FORK_H__
#include <sched.h>
#include <mm.h>
#include <types.h>

#define ENABLE_COPY_ON_WRITE 0
extern int enable_copy_on_write;
struct Process * do_fork(struct Process * p, int clone);

#endif
