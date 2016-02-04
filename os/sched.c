#include <sched.h>
#include <memmap.h>

unsigned int sched_stack[8 * 64];
unsigned sched_active = 0;
unsigned int ticks = 0;
struct Process *cProc;
unsigned int nextPid = 0;

unsigned int ticksToSwitch = 0;

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
void ps() {
    int i;
    printf("Processes:\n");
    for (i = 0; i < 4; i++) {
        printf("Entry %d: state %d pid %d bank %d ap 0x%04x bp 0x%04x sp "
               "0x%04x pc 0x%04x\n",
               i, procs[i].state, procs[i].pid, procs[i].memBank, procs[i].ap,
               procs[i].bp, procs[i].sp, procs[i].pc);
    }
    printf("\n");
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
        procs[i].state = PROC_STATE_NEW;
    } else {
        procs[i].ap = p->ap;
        procs[i].bp = p->bp;
        procs[i].sp = p->sp;
        procs[i].pc = p->pc;
        procs[i].state = p->state;
    }

    printf("Proc pid %d entry %d added\n", pid, i);
    ps();

    return &procs[i];
}

void sched_start() {
    ticksToSwitch = 10;
}

void timer_interrupt(struct IntFrame *fr) {
            int nextTask;
            printf("Switch from PC 0x%04x\n", fr->pc);
            nextTask = currentTask + 1;
            while (nextTask != currentTask) {
                if (nextTask >= MAXPROC)
                    nextTask = 0;

                if (procs[nextTask].state == PROC_STATE_RUN)
                    break;

                if (procs[nextTask].state == PROC_STATE_NEW)
                    break;

                nextTask++;
            }
            if ((nextTask == currentTask) &&
                (procs[currentTask].state == PROC_STATE_RUN)) {
                return;
            }
            if (currentTask < MAXPROC) {
                if (procs[currentTask].state == PROC_STATE_RUN) {
                    procs[currentTask].ap = fr->ap;
                    procs[currentTask].bp = fr->bp;
                    procs[currentTask].sp = fr->sp;
                    procs[currentTask].pc = fr->pc;
                }
            }
            if (procs[nextTask].state == PROC_STATE_NEW) {
                procs[nextTask].state = PROC_STATE_RUN;
            }

            fr->ap = procs[nextTask].ap;
            fr->bp = procs[nextTask].bp;
            fr->sp = procs[nextTask].sp;
            fr->pc = procs[nextTask].pc;
            BANK_SEL = procs[nextTask].memBank;
            printf("Sched switch %d -> %d\n", currentTask, nextTask);
            printf("Switch to PC 0x%04x\n", fr->pc);
            currentTask = nextTask;
            cProc = &(procs[nextTask]);
            ticksToSwitch = 10;
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
