#include <kernel_worker.h>
#include <sched.h>
#include <syscall.h>

struct KernelTask kernelTaskQueue[MAX_QUEUE_SIZE];

#define FORK_BUFFER_SIZE 64 * 64

unsigned int forkBuffer[FORK_BUFFER_SIZE];
unsigned int kernel_worker_stack[64 * 64];

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

void kernel_worker_entry() {
    kernel_worker();
}

void kernel_worker_init() {
    int i = 0;
    struct Process p;
    for (i = 0; i < MAX_QUEUE_SIZE; i++) {
        kernelTaskQueue[i].type = KERNEL_TASK_NONE;
    }

    p.pid = 0;
    p.state = PROC_STATE_NEW;
    p.ap = p.bp = p.sp = (unsigned int)kernel_worker_stack;
    p.pc = (unsigned int)kernel_worker_entry;
    p.memBank = 0;
    p.cwd = fs_root;
    sched_add_proc(0, 0, &p);
}

extern void ps();

void do_kernel_task_fork(int i) {
    struct Process *p;
    printf("Kernel worker: forking!\n");
    if (findProcByPid(kernelTaskQueue[i].callerPid, &p)) {
        struct forkSyscall *sStruct;
        struct Process *newProcess;
        int currentBank;
        int newBank;
        int newPid;
        // printf("Kworker: di\n");
        di();
        if (!mm_allocSegment(&newBank)) {
            printf("Kernel Worker: No more banks!!\n");
            // panic here!
        }
        // printf("KWorker new bank %d\n", newBank);
        ei();
        currentBank = p->memBank;
        copyBanks(newBank, currentBank);

        di();
        p->state = PROC_STATE_RUN;
        newPid = sched_genPid();
        newProcess = sched_add_proc(newPid, newBank, p);

        // set zero pid retval for child
        BANK_SEL = newBank;
        sStruct = (struct forkSyscall *)(*((
            size_t *)(newProcess->ap))); // syscall struct
                                         // pointer sits in
                                         // first arg in arg
                                         // space
        sStruct->pid = 0;

        // set child pid as retval for parent
        BANK_SEL = currentBank;
        sStruct = (struct forkSyscall *)(*((
            size_t *)(p->ap))); // syscall struct pointer sits
                                // in first arg in arg space
        sStruct->pid = newPid;

        kernelTaskQueue[i].type = KERNEL_TASK_NONE;
        ei();
    } else {
        printf("Kernel Worker: pid %d not found!\n",
               kernelTaskQueue[i].callerPid);
    }
}

#define EXECVE_READ_CHUNK_SIZE 0x1000

void do_kernel_task_execve(int i) {
    struct Process *p;
    printf("Kernel worker: execve!\n");
    if (findProcByPid(kernelTaskQueue[i].callerPid, &p)) {
        struct execSyscall *sStruct;
        FILE *fd;
        size_t cPos = 0x8000;
        di();
        BANK_SEL = p->memBank;
        sStruct = (struct execSyscall *)(*((
            size_t *)(p->ap))); // syscall struct pointer sits
                                // in first arg in arg space

        printf("Execve: loading %s\n", sStruct->filename);
        fd = k_open(sStruct->filename, 'r');
        ei();
        while (!k_isEOF(fd)) {
            di();
            BANK_SEL = p->memBank;
            cPos += k_read(fd, (unsigned int *)cPos, EXECVE_READ_CHUNK_SIZE);
            ei();
        }

        di();
        p->pc = 0x8000;
        p->sp = p->bp = p->ap = 0xC000;
        p->state = PROC_STATE_RUN;

        kernelTaskQueue[i].type = KERNEL_TASK_NONE;
        ei();

    } else {
        // hmmm...
    }
}



void do_kernel_task_waitpid(int i) {
    struct Process *p;
    printf("Kernel worker: waitpid!\n");
    if (findProcByPid(kernelTaskQueue[i].callerPid, &p)) {
        struct waitpidSyscall *sStruct;
        struct Process *childProcess;
        int retval = 0;
        sStruct = (struct waitpidSyscall *)(*((
            size_t *)(p->ap))); // syscall struct pointer sits
                                // in first arg in arg space
        di();
        BANK_SEL = p->memBank; 
        retval = findProcByPid(sStruct->pid, &childProcess);
        ei();


        if(!retval) {
            printf("process not found %d\n", sStruct->pid);
            return;
        }

        if(childProcess->state != PROC_STATE_ZOMBIE) {
            printf("process not dead\n");
           return;
        }
        

        di();
        mm_freeSegment(childProcess->memBank);
        childProcess->state = PROC_STATE_NONE;
        p->state = PROC_STATE_RUN;
        sStruct->pid = childProcess->retval;
        kernelTaskQueue[i].type = KERNEL_TASK_NONE;
        ei();

    } else {
        // hmmm...
    }
}

void do_kernel_task_exit(int i) {
    struct Process *p;
    printf("Kernel worker: exit!\n");
    if (findProcByPid(kernelTaskQueue[i].callerPid, &p)) {
        struct exitSyscall *sStruct;
        di();
        BANK_SEL = p->memBank;
        sStruct = (struct exitSyscall *)(*((size_t *)(p->ap)));
        p->retval = sStruct->code;
        p->state = PROC_STATE_ZOMBIE;
        printf("Exit code: %d\n", p->retval);
        kernelTaskQueue[i].type = KERNEL_TASK_NONE;
        ei();
    }
}
void kernel_worker() {
    while (1) {
        int i = 0;
        for (i = 0; i < MAX_QUEUE_SIZE; i++) {
            if (kernelTaskQueue[i].type != KERNEL_TASK_NONE) {
                if (kernelTaskQueue[i].type == KERNEL_TASK_FORK) {
                    do_kernel_task_fork(i);
                } else if (kernelTaskQueue[i].type == KERNEL_TASK_EXECVE) {
                    do_kernel_task_execve(i);
                } else if (kernelTaskQueue[i].type == KERNEL_TASK_EXIT) {
                    do_kernel_task_exit(i);
                } else if (kernelTaskQueue[i].type == KERNEL_TASK_WAITPID) {
                    do_kernel_task_waitpid(i);
                }
            }
        }
    }
}

void addKernelTask(unsigned int task, unsigned int callerPid, void *args) {
    int i = 0;
    for (i = 0; i < MAX_QUEUE_SIZE; i++) {
        if (kernelTaskQueue[i].type == KERNEL_TASK_NONE) {
            break;
        }
    }

    if (i == MAX_QUEUE_SIZE)
        return;

    kernelTaskQueue[i].type = task;
    kernelTaskQueue[i].callerPid = callerPid;
    kernelTaskQueue[i].args = args;
}
