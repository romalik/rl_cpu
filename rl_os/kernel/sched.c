#include <sched.h>
#include <memmap.h>
#include <kernel_worker.h>

#define EXEC_READ_CHUNK 0x1000
#define ARGV_BUFFER_SIZE 256
#define SCHED_STACK_SIZE 512


#define TIMESLICE 25

#define STACK_PLACEMENT 0xe000

unsigned int argvBuffer[ARGV_BUFFER_SIZE];
unsigned int sched_stack[SCHED_STACK_SIZE];
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
        procs[i].isThread = 0;
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
        printf("Entry %d: state %d pid %d bank %d %d ap 0x%04x bp 0x%04x sp "
               "0x%04x pc 0x%04x cmd %s\n",
               i, procs[i].state, procs[i].pid, procs[i].codeMemBank, procs[i].dataMemBank, procs[i].ap,
               procs[i].bp, procs[i].sp, procs[i].pc, procs[i].cmd);
    }
    printf("\n");
}

struct Process *sched_add_proc(unsigned int pid, unsigned int codeBank, unsigned int dataBank,
                               struct Process *p) {
    int i = 0;
    int j;
    for (i = 0; i < MAXPROC; i++) {
        if (procs[i].state == PROC_STATE_NONE) {
            break;
        }
    }

    if (i == MAXPROC)
        return;

    procs[i].pid = pid;
    procs[i].codeMemBank = codeBank;
    procs[i].dataMemBank = dataBank;
    procs[i].isThread = 0;
    if (!p) {
        procs[i].ap = STACK_PLACEMENT;
        procs[i].bp = STACK_PLACEMENT;
        procs[i].sp = STACK_PLACEMENT;
        procs[i].pc = 0x8000;
        procs[i].state = PROC_STATE_NEW;
        if(codeBank == dataBank) {
            procs[i].mode = 0;
        } else {
            procs[i].mode = 1;
        }
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

        procs[i].s = p->s;
        procs[i].d = p->d;

        procs[i].state = p->state;
        
        procs[i].mode = p->mode;


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

    // printf("Proc pid %d entry %d added\n", pid, i);

    // ps();

    return &procs[i];
}

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
            procs[currentTask].s = fr->s;
            procs[currentTask].d = fr->d;
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


    // restore process
    fr->ap = procs[nextTask].ap;
    fr->bp = procs[nextTask].bp;
    fr->sp = procs[nextTask].sp;
    fr->pc = procs[nextTask].pc;
    fr->s = procs[nextTask].s;
    fr->d = procs[nextTask].d;

    CODE_BANK_SEL = procs[nextTask].codeMemBank;
    DATA_BANK_SEL = procs[nextTask].dataMemBank;
//    printf("Sched switch %d -> %d\n", currentTask, nextTask);
//    printf("Switch to PC 0x%04x\n", fr->pc);
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


unsigned int do_exec(struct Process * p, const char * filename, const char ** argv, const char ** envp) {
    FILE *fd;
    unsigned int header[7];
    unsigned int cnt = 0;
    unsigned int mode;
    unsigned int sizeText;
    unsigned int sizeData;
    int bank;
    size_t off;


    fd = k_open(filename, 'r');
    if(!fd) {
        return 1;
    }

    while(cnt != 7) {
        cnt += k_read(fd, header, (7-cnt));
        if(k_isEOF(fd)) {
            k_close(fd);
            return 1;
        }
    }

    if(memcmp(header, &"REXE", 4)) {
        k_close(fd);
        return 1;
    }

    mode = header[4];
    sizeText = header[5];
    sizeData = header[6];
  
    //printf("Loading bin %s header OK mode %d text %d data %d\n", filename, mode, sizeText, sizeData);

    off = parseArgs(argv, argvBuffer, STACK_PLACEMENT);

    if(mode == 0) { // One-segment binary
        unsigned int cPos = 0x8000;
        if(p->mode != 0) {
            mm_freeSegment(p->dataMemBank);
            p->dataMemBank = p->codeMemBank;
        }
        bank = p->codeMemBank;
        cnt = 0;
        while(cnt < sizeText + sizeData) {
            DATA_BANK_SEL = bank;
            cnt += k_read(fd, (unsigned int *)cPos, EXEC_READ_CHUNK);
            cPos = 0x8000 + cnt;
        }
        k_close(fd);
        CODE_BANK_SEL = bank;
        DATA_BANK_SEL = bank;
        memcpy((void *)STACK_PLACEMENT, argvBuffer, off);
        p->pc = 0x8000;
        p->sp = STACK_PLACEMENT + off;
        p->bp = p->ap = STACK_PLACEMENT;
        p->mode = mode;

        memcpy((unsigned int *)p->cmd, (unsigned int *)filename, 32);
        return 0;

    } else if(mode == 1) {
        unsigned int cPos = 0x8000;
        unsigned int dSeg;
        unsigned int cSeg;
        if(p->mode != 1) {
            mm_allocSegment(&dSeg);
            p->dataMemBank = dSeg;
        } else {
            dSeg = p->dataMemBank;
        }
        cSeg = p->codeMemBank;
        cnt = 0;
        while(cnt < sizeText) {
            size_t to_read = EXEC_READ_CHUNK;
            if(sizeText - cnt < EXEC_READ_CHUNK) {
                to_read = sizeText - cnt;
            }
            DATA_BANK_SEL = cSeg;
            cnt += k_read(fd, (unsigned int *)cPos, to_read);
            cPos = 0x8000 + cnt;
        }
        cPos = 0x8000;
        cnt = 0;
        while(cnt < sizeData) {
            size_t to_read = EXEC_READ_CHUNK;
            if(sizeData - cnt < EXEC_READ_CHUNK) {
                to_read = sizeData - cnt;
            }
            DATA_BANK_SEL = dSeg;
            cnt += k_read(fd, (unsigned int *)cPos, to_read);
            cPos = 0x8000 + cnt;
        }
        k_close(fd);
        DATA_BANK_SEL = dSeg;
        memcpy((void *)STACK_PLACEMENT, argvBuffer, off);
        p->pc = 0x8000;
        p->sp = STACK_PLACEMENT + off;
        p->bp = p->ap = STACK_PLACEMENT;
        p->mode = mode;

        memcpy((unsigned int *)p->cmd, (unsigned int *)filename, 32);
        return 0;

    } else {
        return 1;
    }
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
