#include <kernel_worker.h>
#include <sched.h>
#include <syscall.h>
#include <lock.h>
#include <wait.h>
#include <fork.h>
#include <execve.h>

unsigned int kernelTaskQueueLock;
struct KernelTask kernelTaskQueue[MAX_QUEUE_SIZE];

#define FORK_BUFFER_SIZE 4096
#define KERNEL_WORKER_STACK_SIZE 256

//#define FORK_BUFFER_SIZE 64*64
//#define KERNEL_WORKER_STACK_SIZE 64*4

unsigned int forkBuffer[FORK_BUFFER_SIZE];
unsigned int kernel_worker_stack[KERNEL_WORKER_STACK_SIZE];

/*
void copyBanks(unsigned int dest, unsigned int src) {
    unsigned int copied = 0;
    while (copied < 0x8000) {
        di();
        // printf("Copying pages.. di\n", copied);
        mm_memcpy(forkBuffer, (unsigned int *)(0x8000 + copied),
                  FORK_BUFFER_SIZE, src);
        mm_memcpy((unsigned int *)(0x8000 + copied), forkBuffer,
                  FORK_BUFFER_SIZE, dest);
        copied += FORK_BUFFER_SIZE;
        // printf("Copying pages.. 0x%04x completed\n", copied);
        ei();
    }
}
*/
void kernel_worker_entry() {
    kernel_worker();
}

void kernel_worker_init() {
    int i = 0;
    struct Process * p;
    size_t off;
    //spinlock_init(&kernelTaskQueueLock);
    for (i = 0; i < MAX_QUEUE_SIZE; i++) {
        kernelTaskQueue[i].type = KERNEL_TASK_NONE;
    }

    p = get_free_proc();

    strcpy(p->cmd, "[kernel_worker]");

    p->pid = 0;
    p->state = PROC_STATE_RUN;
    p->mmuSelector = 0;
    p->cwd = fs_root;
    cProc = p;

}

extern void ps();

#define offsetof(st, m) ((size_t)&(((st *)0)->m))

void do_kernel_task_fork(int i) {
    struct Process *p;
    struct Process *new_p;
    printf("Kernel worker: forking!\n");
    if (findProcByPid(kernelTaskQueue[i].callerPid, &p)) {
        size_t parentSyscallStructAddr;
        new_p = do_fork(p, 0);
        printf("Kernel worker: new_p 0x%04X\n", new_p);
        //fill parent info, child's should be zeroed by library!!


        //parentSyscallStructAddr = ugetc(p, p->sp + (offsetof(struct InterruptFrame, AP) - offsetof(struct InterruptFrame, __sp_ptr)), 0, 0xe);
        //uputc(p, parentSyscallStructAddr + offsetof(struct forkSyscall, pid), 0, 0xe, new_p->pid);

        printf("New pid %d, mmuSelector %d\n", new_p->pid, new_p->mmuSelector);
        kernelTaskQueue[i].type = KERNEL_TASK_NONE;
        p->state = PROC_STATE_RUN;

    }

    printf("Forked!\n");
    /*
        struct forkSyscall *sStruct;
        struct Process *newProcess;
        int currentCodeBank;
        int currentDataBank;
        int newCodeBank;
        int newDataBank;
        int newPid;
        //printf("Kworker: di\n");
        di();
        currentCodeBank = p->codeMemBank;
        currentDataBank = p->dataMemBank;
        if (!mm_allocSegment(&newCodeBank)) {
            printf("Kernel Worker: No more banks!!\n");
            // panic here!
        }
        if(currentCodeBank == currentDataBank) {
            newDataBank = newCodeBank;
        } else {
            if (!mm_allocSegment(&newDataBank)) {
                printf("Kernel Worker: No more banks!!\n");
                // panic here!
            }

        }

        //printf("KWorker new bank %d\n", newBank);
        ei();
        copyBanks(newCodeBank, currentCodeBank);
        if(newDataBank != newCodeBank) {
            copyBanks(newDataBank, currentDataBank);
        }
        

        di();
        p->state = PROC_STATE_RUN;
        newPid = sched_genPid();
        newProcess = sched_add_proc(newPid, newCodeBank, newDataBank, p);

        // set zero pid retval for child
        DATA_BANK_SEL = newDataBank;
        sStruct = (struct forkSyscall *)(*((
            size_t *)(newProcess->ap))); // syscall struct
                                         // pointer sits in
                                         // first arg in arg
                                         // space
        sStruct->pid = 0;
        newProcess->parent = p;

        // set child pid as retval for parent
        DATA_BANK_SEL = currentDataBank;
        sStruct = (struct forkSyscall *)(*((
            size_t *)(p->ap))); // syscall struct pointer sits
                                // in first arg in arg space
        sStruct->pid = newPid;

        ei();
    } else {
        printf("Kernel Worker: pid %d not found!\n",
               kernelTaskQueue[i].callerPid);
    }
*/
}

void do_kernel_task_clone(int i) {
    struct Process *p;
    struct Process *new_p;
    printf("Kernel worker: cloning!\nSTUB\n");
    while(1) {}
/*
    if (findProcByPid(kernelTaskQueue[i].callerPid, &p)) {
        size_t parentSyscallStructAddr;
        new_p = do_fork(p, 1);
        //fill parent info, child's should be zeroed by library!!

        parentSyscallStructAddr = ugetc(p, p->ap, 0, 0xe);
        uputc(p, parentSyscallStructAddr + offsetof(struct forkSyscall, pid), 0, 0xe, new_p->pid);

        printf("New pid %d, mmuSelector %d\n", new_p->pid, new_p->mmuSelector);
        kernelTaskQueue[i].type = KERNEL_TASK_NONE;
        run_proc(p);

    }

    printf("Cloned!\n");
  */
  /*
    struct Process *p;
    if (findProcByPid(kernelTaskQueue[i].callerPid, &p)) {
        struct cloneSyscall *sStruct;
        struct Process *newProcess;
        int currentCodeBank;
        int currentDataBank;
        int newPid;




        di();
        currentCodeBank = p->codeMemBank;
        currentDataBank = p->dataMemBank;
        DATA_BANK_SEL = currentDataBank;
        sStruct = (struct cloneSyscall *)(*((
            size_t *)(p->ap))); // syscall struct pointer sits
                                // in first arg in arg space
        p->state = PROC_STATE_RUN;
        newPid = sched_genPid();
        newProcess = sched_add_proc(newPid, currentCodeBank, currentDataBank, p);

        // set zero pid retval for child
        newProcess->parent = p;
        *(unsigned int *)(sStruct->stack) = (unsigned int)sStruct->args;
        newProcess->ap = (unsigned int)sStruct->stack;
        newProcess->bp = newProcess->sp = ((unsigned int)sStruct->stack + 1);
        newProcess->pc = (unsigned int)(sStruct->fn);
    newProcess->mpc = 0;
        newProcess->isThread = 1;
        newProcess->state = PROC_STATE_NEW;
        kernelTaskQueue[i].type = KERNEL_TASK_NONE;
        sStruct->retval = newPid;
        ei();
    } else {
        printf("Kernel Worker: pid %d not found!\n",
               kernelTaskQueue[i].callerPid);
    }
*/
}



void do_kernel_task_execve(int i, void * args) {
    struct Process *p;
    struct execSyscall *sStruct = args;
    printf("EXECVE\n");
    if (findProcByPid(kernelTaskQueue[i].callerPid, &p)) {

      printf("EXECVE for pid %d\n", p->pid);

      di();
        do_execve(p, sStruct->filename, sStruct->argv, sStruct->envp);
      ei();
        kernelTaskQueue[i].type = KERNEL_TASK_NONE;
        run_proc(p);
    }
    //while(1) {}
    /*
    struct Process *p;
    if (findProcByPid(kernelTaskQueue[i].callerPid, &p)) {
        struct execSyscall *sStruct;
        di();
        DATA_BANK_SEL = p->dataMemBank;
        sStruct = (struct execSyscall *)(*((
            size_t *)(p->ap))); // syscall struct pointer sits
                                // in first arg in arg space



        do_exec(p, sStruct->filename, sStruct->argv, sStruct->envp);

        p->state = PROC_STATE_RUN;

        kernelTaskQueue[i].type = KERNEL_TASK_NONE;
        ei();

    } else {
     printf("Kernel worker: execve TROUBLES!\n");
        // hmmm...
    }
*/
}

void do_kernel_task_waitpid(int i) {
    printf("WAITPID\n");
    while(1) {}
    /*
    struct Process *p;
    //printf("Kernel worker: waitpid!\n");
    if (findProcByPid(kernelTaskQueue[i].callerPid, &p)) {
        struct waitpidSyscall *sStruct;
        struct Process *childProcess;
        int pid = 0;
        int retval = 0;
        int options = 0;
        //printf("Kernel worker: waitpid caller pid %d\n", kernelTaskQueue[i].callerPid);
        di();
        DATA_BANK_SEL = p->dataMemBank;
        sStruct = (struct waitpidSyscall *)(*((
            size_t *)(p->ap))); // syscall struct pointer sits
                                // in first arg in arg space

        if(sStruct->pid > 0) {
          retval = findProcByPid(sStruct->pid, &childProcess);
        } else {
          retval = findProcByParent(p, &childProcess);
        }
        pid = sStruct->pid;
        options = sStruct->options;

        if (!retval) {
            //printf("process not found %d. Caller pid %d\n", pid,p->pid);
            if(options == WNOHANG) {
              p->state = PROC_STATE_RUN;
              kernelTaskQueue[i].type = KERNEL_TASK_NONE;
              sStruct->pid = 0;
            }
            ei();
            resched_now();
            return;
        }

        if (childProcess->state != PROC_STATE_ZOMBIE) {
            //printf("process not dead %d\n", pid);

            if(sStruct->options == WNOHANG) {
              p->state = PROC_STATE_RUN;
              kernelTaskQueue[i].type = KERNEL_TASK_NONE;
              sStruct->pid = 0;
            }
            ei();
            resched_now();
            return;
        }

        childProcess->state = PROC_STATE_NONE;
        p->state = PROC_STATE_RUN;
        sStruct->pid = childProcess->pid;
        *(sStruct->status) = childProcess->retval;
        kernelTaskQueue[i].type = KERNEL_TASK_NONE;
        ei();

    } else {
     printf("Kernel worker: waitpid TROUBLES!\n");
        // hmmm...
    }
*/
}

void do_kernel_task_exit(int i) {
    printf("EXIT\n");
    while(1) {}
    /*
    struct Process *p;
    if (findProcByPid(kernelTaskQueue[i].callerPid, &p)) {
        struct exitSyscall *sStruct;
        int i;
        di();
        DATA_BANK_SEL = p->dataMemBank;
        sStruct = (struct exitSyscall *)(*((size_t *)(p->ap)));
        p->retval = sStruct->code;
        p->state = PROC_STATE_ZOMBIE;
        //printf("Exit code: %d\n", p->retval);
        kernelTaskQueue[i].type = KERNEL_TASK_NONE;
        //close files
        for (i = 0; i < MAX_FILES_PER_PROC; i++) {
            if (p->openFiles[i]) {
              k_close(p->openFiles[i]);
              p->openFiles[i] = 0;
              //printf("Closing file %d\n", i);
            }
        }

        if(!p->isThread) {
            mm_freeSegment(p->codeMemBank);
            mm_freeSegment(p->dataMemBank);
        }
        ei();
    } else {

        kernelTaskQueue[i].type = KERNEL_TASK_NONE;
      //printf("Kernel worker: EXIT TROUBLES!!!! double exit??\n");
    }
*/
}
void kernel_worker() {
    while (1) {
        int i = 0;
        for (i = 0; i < MAX_QUEUE_SIZE; i++) {
            //spinlock_lock(&kernelTaskQueueLock);
            if (kernelTaskQueue[i].type != KERNEL_TASK_NONE) {
                if (kernelTaskQueue[i].type == KERNEL_TASK_FORK) {
                    do_kernel_task_fork(i);
                } else if (kernelTaskQueue[i].type == KERNEL_TASK_EXECVE) {
                    do_kernel_task_execve(i, kernelTaskQueue[i].args);
                } else if (kernelTaskQueue[i].type == KERNEL_TASK_EXIT) {
                    do_kernel_task_exit(i);
                } else if (kernelTaskQueue[i].type == KERNEL_TASK_WAITPID) {
                    do_kernel_task_waitpid(i);
                } else if (kernelTaskQueue[i].type == KERNEL_TASK_CLONE) {
                    do_kernel_task_clone(i);
                }
            }
            //spinlock_unlock(&kernelTaskQueueLock);
        }
    }
}

void showTasks() {
    int i =0;
    for(i = 0; i<MAX_QUEUE_SIZE; i++) {
        printf("Task type %d caller %d\n", kernelTaskQueue[i].type, kernelTaskQueue[i].callerPid);
    }
}


void addKernelTask(unsigned int task, unsigned int callerPid, void *args) {
    int i = 0;

    //spinlock_lock(&kernelTaskQueueLock);


    for (i = 0; i < MAX_QUEUE_SIZE; i++) {
        if (kernelTaskQueue[i].type == KERNEL_TASK_NONE) {
            break;
        }
    }

    if (i == MAX_QUEUE_SIZE) {
        //spinlock_unlock(&kernelTaskQueueLock);
        return;
    }

    kernelTaskQueue[i].type = task;
    kernelTaskQueue[i].callerPid = callerPid;
    kernelTaskQueue[i].args = args;
    //spinlock_unlock(&kernelTaskQueueLock);

    //    showTasks();
}

