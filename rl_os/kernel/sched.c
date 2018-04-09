#include <sched.h>
#include <memmap.h>
#include <kernel_worker.h>
#include <lock.h>
#include <mm.h>


#define SCHED_STACK_SIZE 512



unsigned int TIMESLICE = 20;

//#define TIMESLICE 20

#define STACK_PLACEMENT 0xe000

unsigned int sched_stack[SCHED_STACK_SIZE];
unsigned sched_active = 0;
unsigned int ticks = 0;
struct Process *cProc;
unsigned int nextPid = 0;
unsigned int nr_ready = 1;
unsigned int ticksToSwitch = 0;

struct Process procs[MAXPROC];
unsigned int currentTask = 0;

extern unsigned int kernel_worker_stack[];
extern void kernel_worker_entry();

unsigned int schedMx;

void resched();


void timer_interrupt() {
  sched_stack[0] =  cProc->mmuSelector;
    ticks++;
    if(ticksToSwitch) {
        ticksToSwitch--;
        if(ticksToSwitch == 0) resched();
    }
}

void sched_init() {
    int i = 0;

    spinlock_init(&schedMx);

    for (i = 0; i < MAXPROC; i++) {
        procs[i].state = PROC_STATE_NONE;
        procs[i].signalsPending = 0;
        procs[i].isThread = 0;
    }
}

void printProcess(struct Process * p) {
    int i = 0;
    printf("Process: ptr 0x%04x\npid: %d\nmmu: %d\nsp: 0x%04X\nstate: %d\n", p, p->pid, p->mmuSelector, p->sp, p->state);

    for(i = 0; i<20; i++) {
        printf("code page %d : 0x%04X\n", i, mmu_read_table(p->mmuSelector, i, 1));
    }
    for(i = 0; i<16; i++) {
        printf("data page %d : 0x%04X\n", i, mmu_read_table(p->mmuSelector, i, 0));
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

        printf("Entry %d: state %d pid %d sp "
               "0x%04x cmd %s\n",
               i, procs[i].state, procs[i].pid, procs[i].sp, procs[i].cmd);
    }
    printf("\n");
}

struct Process *get_free_proc() {
    int i = 0;
    int j;
    spinlock_lock(&schedMx);

    for (i = 0; i < MAXPROC; i++) {
        if (procs[i].state == PROC_STATE_NONE) {
            break;
        }
    }

    if (i == MAXPROC) {
        printf("MAXPROC!\n");
        spinlock_unlock(&schedMx);
        return 0;
    }
    procs[i].state = PROC_STATE_CONSTRUCT;
    spinlock_unlock(&schedMx);

//    printf("Get free proc [%d] 0x%04X\n", i, &procs[i]);
    return &procs[i];
}

void run_proc(struct Process * p) {
    p->state = PROC_STATE_NEW;
}
/*
struct Process *sched_add_proc(unsigned int pid, unsigned int mmuSelector, struct Process *p) {
    int i = 0;
    int j;
    spinlock_lock(&schedMx);

    for (i = 0; i < MAXPROC; i++) {
        if (procs[i].state == PROC_STATE_NONE) {
            break;
        }
    }

    if (i == MAXPROC) {
        printf("MAXPROC!\n");
        spinlock_unlock(&schedMx);
        return 0;
    }

    procs[i].pid = pid;
    procs[i].mmuSelector = mmuSelector;
    
    procs[i].isThread = 0;
    procs[i].mode = 1;
    if (!p) {
        procs[i].ap = STACK_PLACEMENT;
        procs[i].bp = STACK_PLACEMENT;
        procs[i].sp = STACK_PLACEMENT;
        procs[i].pc = 0x0000;
        procs[i].mpc = 0;
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
        procs[i].mpc = p->mpc;

        procs[i].s = p->s;
        procs[i].d = p->d;

        procs[i].state = p->state;
        


        memcpy((unsigned int *)(&procs[i].cwd), (unsigned int *)(&p->cwd),
               sizeof(struct fs_node));

        memcpy((unsigned int *)(&procs[i].sigActions), (unsigned int *)(&p->sigActions), sizeof(sighandler_t) * SIGNUM);

        procs[i].signalsPending = p->signalsPending;
        memcpy((unsigned int *)(&procs[i].openFiles),
               (unsigned int *)(&p->openFiles),
               sizeof(FILE *) * MAX_FILES_PER_PROC);

        for(j = 0; j<MAX_FILES_PER_PROC; j++) {
            procs[i].openFiles[j]->refcnt++;
        }


        memcpy((unsigned int *)(&procs[i].cmd),
               (unsigned int *)(&p->cmd),
               32);
    }

    //printf("Proc pid %d entry %d pc 0x%04X added\n", pid, i, procs[i].pc);

    //ps();
    spinlock_unlock(&schedMx);

    return &procs[i];
}
*/


void sched_start() {
    ticksToSwitch = TIMESLICE;
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
    spinlock_lock(&schedMx);
    r = findProcByPid(pid, &pt);
    if(r) {
        pt->signalsPending |= (1 << sig);
    }
    spinlock_unlock(&schedMx);
    return 0;
}

extern size_t __sp_before_int;
void resched() {
    int nextTask;

//    printf("__sp_before_int 0x%04X, addr = 0x%04X\n", __sp_before_int, &__sp_before_int);



    // check if we have any pending tasks
    //if(nr_ready < 2) { //nowhere to switch
    //    return;
    //}

    // now save current task state to its ptab
    // if current task is valid
    if (currentTask < MAXPROC) {
        if (procs[currentTask].state == PROC_STATE_RUN || procs[currentTask].state == PROC_STATE_SLEEP ) {
            //	printf("Switching from %d pc 0x%04X mpc 0x%04X\n", currentTask, fr->pc, fr->mpc);
/*
            procs[currentTask].ap = fr->ap;
            procs[currentTask].bp = fr->bp;
            procs[currentTask].sp = fr->sp;
            procs[currentTask].pc = fr->pc;
            procs[currentTask].mpc = fr->mpc;
            procs[currentTask].s = fr->s;
            procs[currentTask].d = fr->d;
            */

            procs[currentTask].sp = __sp_before_int;

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

        if (procs[nextTask].state == PROC_STATE_SLEEP && procs[nextTask].signalsPending)
            break;

        nextTask++;
    }

    if(nextTask == currentTask) {
        // hmmm.. if we asserted nr_ready > 1 we should never get here

        sched_stack[0] = procs[nextTask].mmuSelector;
        sched_stack[1] = procs[nextTask].sp;

        return;
    }

    // check for signals

    if(procs[nextTask].signalsPending) {
        int sig = getSignalFromMask(procs[nextTask].signalsPending);
        //printf("Found signal %d while waking process %d. Mask = 0x%04x\n", sig, procs[nextTask].pid, procs[nextTask].signalsPending);
        if(sig == SIGKILL) {
            //printf("SIGKILL! killing pid %d\n", procs[nextTask].pid);
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


    //sched_stack[0] - mmuSelector
    //sched_stack[1] - sp

    // restore process
    sched_stack[0] = procs[nextTask].mmuSelector;
    sched_stack[1] = procs[nextTask].sp;
/*
    CODE_BANK_SEL = procs[nextTask].codeMemBank;
    DATA_BANK_SEL = procs[nextTask].dataMemBank;
*/
    //printf("Sched switch %d -> %d\n", currentTask, nextTask);
    //printf("Switch to PC 0x%04x\n", fr->pc);

//    printf("Switch\npid %d -> %d\nmmuSel %d -> %d\nsp %d -> %d\n", procs[currentTask].pid, procs[nextTask].pid, procs[currentTask].mmuSelector, procs[nextTask].mmuSelector, procs[currentTask].sp, procs[nextTask].sp);

//    printf("Curr proc: "); printProcess(&procs[currentTask]);
//    printf("Next proc: "); printProcess(&procs[nextTask]);


    currentTask = nextTask;
    cProc = &(procs[nextTask]);
    ticksToSwitch = TIMESLICE;



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
unsigned int findProcByParent(struct Process * pid, struct Process **p) {
    int i;
    for (i = 0; i < MAXPROC; i++) {
        if (procs[i].parent == pid && (procs[i].state != PROC_STATE_NONE)) {
            *p = &procs[i];
            return 1;
        }
    }
    return 0;
}
/*
size_t parseArgs(const char **nArgv, unsigned int *buf, size_t off) {
    unsigned int *argc;
    unsigned int *argv;
    unsigned int *p;
    argc = buf;
    argv = buf + 2;
    p = buf + 0x12;

    (*argc) = 0;
    *(buf + 1) = off + 2;

    if(!nArgv) {
        return 0x12;
    }

    while (*nArgv) {
        (*argc)++;
        strcpy(p, *nArgv);
        *argv = p - buf + off;
        argv++;
        p += strlen(*nArgv) + 1;
        nArgv++;
    }
    *argv = 0;
    return (size_t)p - (size_t)buf;
}
*/


void sleep(struct Process * proc, void * event) {
    spinlock_lock(&schedMx);
    proc->state = PROC_STATE_SLEEP;
    proc->waitingOn = event;
    printf("Sleep process %d on 0x%04x\n", proc->pid, (size_t)(event));
    spinlock_unlock(&schedMx);
}


void wakeup(void * event) {
    int i;
    spinlock_lock(&schedMx);
    printf("Try wake on 0x%04x\n", (size_t)(event));
    for(i = 0; i<MAXPROC; i++) {
        if(procs[i].state == PROC_STATE_SLEEP) {
            if(procs[i].waitingOn == event) {
                procs[i].state = PROC_STATE_RUN;
                procs[i].waitingOn = 0;
                printf("Wake process %d\n", procs[i].pid);
            }
        }

    }
    spinlock_unlock(&schedMx);
}





unsigned int proc_file_read(unsigned int minor, unsigned int * buf, size_t n) {
    int i;
    unsigned int * b = buf;
    //printf("procfs read\n");
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

unsigned int sched_file_read(unsigned int minor, unsigned int * buf, size_t n) {
    return 0;
}

unsigned int sched_file_write(unsigned int minor, const unsigned int * buf, size_t n) {
    TIMESLICE = *buf;
    return n;
}

