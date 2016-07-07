#ifndef SCHED_H__
#define SCHED_H__
#include <types.h>
#include <memmap.h>
#include <rlfs3.h>

#define MAXPROC 15
#define MAX_FILES_PER_PROC 15

#define PROC_STATE_NONE 0
#define PROC_STATE_RUN 1
#define PROC_STATE_WAIT 2
#define PROC_STATE_ZOMBIE 3
#define PROC_STATE_KWORKER 4
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

    struct Process *parent;

    int retval;

    FILE *openFiles[MAX_FILES_PER_PROC];
};

extern struct Process *cProc;
extern struct Process procs[MAXPROC];

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

void resched_now();

#endif
