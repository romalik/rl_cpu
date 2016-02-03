#ifndef SCHED_H__
#define SCHED_H__
#include <types.h>
#include <memmap.h>
#include <rlfs3.h>

#define PROC_STATE_NONE 0
#define PROC_STATE_RUN 1
#define PROC_STATE_WAIT 2
#define PROC_STATE_EXIT 3
#define PROC_STATE_FORKING 4
#define PROC_STATE_NEW 5
extern unsigned int ticks;

struct Process {
    unsigned int pid;
    unsigned int state;
    unsigned int ap;
    unsigned int bp;
    unsigned int sp;
    unsigned int pc;

    unsigned int memBank;
    struct fs_node cwd;
};

extern struct Process *cProc;

struct IntFrame {
    unsigned int sp;
    unsigned int pc;
    unsigned int bp;
    unsigned int ap;
};

unsigned int findProcByPid(unsigned int pid, struct Process **p);
struct Process *sched_add_proc(unsigned int pid, unsigned int bank,
                               struct Process *p);
unsigned int sched_genPid();
#define MAXPROC 15

#endif
