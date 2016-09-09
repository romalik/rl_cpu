#include <sched.h>
#include <memmap.h>
#include <kernel_worker.h>


unsigned int sched_stack[8 * 64];
unsigned sched_active = 0;
unsigned int ticks = 0;
struct Process *cProc;
unsigned int nextPid = 0;
unsigned int nr_ready = 1;
unsigned int ticksToSwitch = 0;

struct Process procs[MAXPROC];
unsigned int currentTask = -1;

extern unsigned int kernel_worker_stack[];
extern void kernel_worker_entry();

void sched_init() {
    int i = 0;
    for (i = 0; i < MAXPROC; i++) {
        procs[i].state = PROC_STATE_NONE;
        procs[i].signalsPending = 0;
    }
}

unsigned int sched_genPid() {
    nextPid++;
    return nextPid;
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
        procs[i].ap = 0xF000;
        procs[i].bp = 0xF000;
        procs[i].sp = 0xF000;
        procs[i].pc = 0x8000;
        procs[i].state = PROC_STATE_NEW;
        memcpy((unsigned int *)(&procs[i].cwd), (unsigned int *)(&fs_root),
               sizeof(struct fs_node));

        memset((unsigned int *)(&procs[i].sigActions), SIG_DFL, SIGNUM);

        procs[i].signalsPending = 0;

        memset((unsigned int *)(&procs[i].openFiles), 0,
               sizeof(FILE *) * MAX_FILES_PER_PROC);

        memset((unsigned int *)(&procs[i].cmd), 0,
               32);
    } else {
        procs[i].ap = p->ap;
        procs[i].bp = p->bp;
        procs[i].sp = p->sp;
        procs[i].pc = p->pc;
        procs[i].state = p->state;
        memcpy((unsigned int *)(&procs[i].cwd), (unsigned int *)(&p->cwd),
               sizeof(struct fs_node));

        memcpy((unsigned int *)(&procs[i].sigActions), (unsigned int *)(&p->sigActions), sizeof(sighandler_t) * SIGNUM);

        procs[i].signalsPending = p->signalsPending;
        memcpy((unsigned int *)(&procs[i].openFiles),
               (unsigned int *)(&p->openFiles),
               sizeof(FILE *) * MAX_FILES_PER_PROC);

        memcpy((unsigned int *)(&procs[i].cmd),
               (unsigned int *)(&p->cmd),
               32);
    }

    // printf("Proc pid %d entry %d added\n", pid, i);

    // ps();

    return &procs[i];
}

void sched_start() {
    ticksToSwitch = 10;
}

void resched_now() {
    ticksToSwitch = 1;
}

unsigned int getSignalFromMask(unsigned int mask) {
    unsigned int i;
    for(i = 0; i<SIGNUM; i++) {
        if(mask & (1 << i)) {
            break;
        }
    }
    return i;
}

unsigned int sendSig(unsigned int pid, unsigned int sig) {
    int r;
    struct Process * pt;
    r = findProcByPid(pid, &pt);
    if(r) {
        pt->signalsPending |= (1 << sig);
    }
    return 0;
}


void resched(struct IntFrame * fr) {
    int nextTask;

    // check if we have any pending tasks
    //if(nr_ready < 2) { //nowhere to switch
    //    return;
    //}

    // now save current task state to its ptab
    // if current task is valid
    if (currentTask < MAXPROC) {
        if (procs[currentTask].state == PROC_STATE_RUN) {
            procs[currentTask].ap = fr->ap;
            procs[currentTask].bp = fr->bp;
            procs[currentTask].sp = fr->sp;
            procs[currentTask].pc = fr->pc;
        }
    }

    // now select next task
    //
    // either ready, or with pending signals
    //

    nextTask = currentTask + 1;

rescanTable:
    while(nextTask != currentTask) {
        if(nextTask > MAXPROC) {
            nextTask = 0;
        }

        if (procs[nextTask].state == PROC_STATE_RUN)
            break;

        if (procs[nextTask].state == PROC_STATE_NEW)
            break;

        if (procs[nextTask].state == PROC_STATE_WAIT && procs[nextTask].signalsPending)
            break;

        nextTask++;
    }

    if(nextTask == currentTask) {
        // hmmm.. if we asserted nr_ready > 1 we should never get here
        return;
    }

    // check for signals

    if(procs[nextTask].signalsPending) {
        int sig = getSignalFromMask(procs[nextTask].signalsPending);
        printf("Found signal %d while waking process %d. Mask = 0x%04x\n", sig, procs[nextTask].pid, procs[nextTask].signalsPending);
        if(sig == SIGKILL) {
          printf("SIGKILL! killing pid %d\n", procs[nextTask].pid);
          addKernelTask(KERNEL_TASK_EXIT, procs[nextTask].pid, NULL);
          procs[nextTask].state = PROC_STATE_KWORKER;
          nextTask++;
          goto rescanTable;
        }
    }

    // if new process - mark it as ready


    if (procs[nextTask].state == PROC_STATE_NEW) {
        procs[nextTask].state = PROC_STATE_RUN;
    }


    // restore process
    fr->ap = procs[nextTask].ap;
    fr->bp = procs[nextTask].bp;
    fr->sp = procs[nextTask].sp;
    fr->pc = procs[nextTask].pc;
    BANK_SEL = procs[nextTask].memBank;
    // printf("Sched switch %d -> %d\n", currentTask, nextTask);
    // printf("Switch to PC 0x%04x\n", fr->pc);
    currentTask = nextTask;
    cProc = &(procs[nextTask]);
    ticksToSwitch = 10;


}


void _resched(struct IntFrame *fr) {
    int nextTask;
    // printf("Switch from PC 0x%04x\n", fr->pc);
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
    // printf("Sched switch %d -> %d\n", currentTask, nextTask);
    // printf("Switch to PC 0x%04x\n", fr->pc);
    currentTask = nextTask;
    cProc = &(procs[nextTask]);
    ticksToSwitch = 10;
}

unsigned int findProcByPid(unsigned int pid, struct Process **p) {
    int i;
    for (i = 0; i < MAXPROC; i++) {
        if (procs[i].pid == pid && (procs[i].state != PROC_STATE_NONE)) {
            *p = &procs[i];
            return 1;
        }
    }
    return 0;
}



unsigned int proc_file_read(unsigned int minor, unsigned int * buf, size_t n) {
  int i;
  unsigned int * b = buf;
  printf("procfs read\n");
  for(i = 0; i<MAXPROC; i++) {
    if(procs[i].state != PROC_STATE_NONE) {

        *b = procs[i].pid;
        b++;
        *b = procs[i].state;
        b++;
        memcpy((unsigned int *)b, (unsigned int *)procs[i].cmd, 32);
        b += 32;
    }
  }
  *b = 0;

  return (unsigned int)b - (unsigned int)buf + 1;
}


unsigned int proc_file_write(unsigned int minor, const unsigned int * buf, size_t n) {
  return 0;
}
