#ifndef FORK_H__
#define FORK_H__
#include <sched.h>
#include <mm.h>
#include <types.h>


struct Process * do_fork(struct Process * p, int clone);

#endif
