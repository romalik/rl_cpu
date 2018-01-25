#ifndef FORK_H__
#define FORK_H__
#include <sched.h>
#include <mm.h>
#include <types.h>


int do_fork(struct Process * p);

#endif
