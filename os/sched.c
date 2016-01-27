#include <sched.h>
#include <memmap.h>

unsigned int sched_stack[8 * 64];
unsigned sched_active = 0;
unsigned int ticks = 0;
struct Process *cProc;
unsigned int nextPid = 0;

struct Process procs[MAXPROC];
unsigned int currentTask = MAXPROC;


extern unsigned int kernel_worker_stack[];
extern void kernel_worker_entry();

void sched_init() {
    int i = 0;
    for (i = 0; i < MAXPROC; i++) {
        procs[i].state = PROC_STATE_NONE;
    }
}

unsigned int sched_genPid() {
    nextPid++;
    return nextPid;
}

struct Process *sched_add_proc(unsigned int pid, unsigned int bank,
                               struct Process *p) {
    int i = 0;
    for (i = 0; i < MAXPROC; i++) {
        if (procs[i].state == PROC_STATE_NONE) {
            break;
        }
    }

    if (i == MAXPROC)
        return;

    procs[i].pid = pid;
    procs[i].memBank = bank;
    if (!p) {
        procs[i].ap = 0xC000;
        procs[i].bp = 0xC000;
        procs[i].sp = 0xC000;
        procs[i].pc = 0x8000;
    } else {
        procs[i].ap = p->ap;
        procs[i].bp = p->bp;
        procs[i].sp = p->sp;
        procs[i].pc = p->pc;
    }
    procs[i].state = PROC_STATE_RUN;

    return &procs[i];
}
void ps() {
    int i;
    printf("Processes:\n");
    for (i = 0; i < MAXPROC; i++) {
        printf("Entry %d: state %d pid %d bank %d ap 0x%04x bp 0x%04x sp "
               "0x%04x pc 0x%04x\n",
               i, procs[i].state, procs[i].pid, procs[i].memBank, procs[i].ap,
               procs[i].bp, procs[i].sp, procs[i].pc);
    }
    printf("\n");
}

void sched_start() {
    sched_active = 1;
}

void timer_interrupt(struct IntFrame *fr) {
    int nextTask;
    ticks++;

    if (sched_active) {
        di();
        nextTask = currentTask + 1;
        while (nextTask != currentTask) {
            if (nextTask == MAXPROC)
                nextTask = 0;

            if (procs[nextTask].state == PROC_STATE_RUN)
                break;

            nextTask++;
        }
        if (procs[currentTask].state == PROC_STATE_RUN) {
            procs[currentTask].ap = fr->ap;
            procs[currentTask].bp = fr->bp;
            procs[currentTask].sp = fr->sp;
            procs[currentTask].pc = fr->pc;
        }

        fr->ap = procs[nextTask].ap;
        fr->bp = procs[nextTask].bp;
        fr->sp = procs[nextTask].sp;
        fr->pc = procs[nextTask].pc;
        BANK_SEL = procs[nextTask].memBank;
        printf("Sched switch %d -> %d\n", currentTask, nextTask);
        currentTask = nextTask;
        cProc = &(procs[nextTask]);
        ei();

    }
}

unsigned int findProcByPid(unsigned int pid, struct Process **p) {
    int i;
    for (i = 0; i < MAXPROC; i++) {
        if (procs[i].pid == pid) {
            *p = &procs[i];
            return 1;
        }
    }
    return 0;
}
