#include <kernel_worker.h>
#include <sched.h>

struct KernelTask kernelTaskQueue[MAX_QUEUE_SIZE];

#define FORK_BUFFER_SIZE 64 * 64

unsigned int forkBuffer[FORK_BUFFER_SIZE];
unsigned int kernel_worker_stack[64 * 64];

void copyBanks(unsigned int dest, unsigned int src) {
    unsigned int copied = 0;
    while (copied < 0x8000) {
        printf("Copying pages.. di\n", copied);
        di();
        mm_memcpy(forkBuffer, (unsigned int *)(0x8000 + copied),
                  FORK_BUFFER_SIZE, src);
        mm_memcpy((unsigned int *)(0x8000 + copied), forkBuffer,
                  FORK_BUFFER_SIZE, dest);
        copied += FORK_BUFFER_SIZE;
        ei();
        printf("Copying pages.. 0x%04x completed\n", copied);
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

void kernel_worker() {
    while (1) {
        int i = 0;
        for (i = 0; i < MAX_QUEUE_SIZE; i++) {
            if (kernelTaskQueue[i].type != KERNEL_TASK_NONE) {
                if (kernelTaskQueue[i].type == KERNEL_TASK_FORK) {
                    struct Process *p;
                    printf("Kernel worker: forking!\n");
                    if (findProcByPid(kernelTaskQueue[i].src, &p)) {
                        struct ForkSyscallStruct {
                            unsigned int id;
                            unsigned int pid;
                        };
                        struct ForkSyscallStruct *sStruct;
                        struct Process *newProcess;
                        int currentBank;
                        int newBank;
                        int newPid;
                        printf("Kworker: di\n");
                        di();
                        if (!mm_allocSegment(&newBank)) {
                            printf("Kernel Worker: No more banks!!\n");
                            // panic here!
                        }
                        printf("KWorker new bank %d\n", newBank);
                        ei();
                        currentBank = p->memBank;
                        copyBanks(newBank, currentBank);

                        di();
                        p->state = PROC_STATE_RUN;
                        newPid = sched_genPid();
                        newProcess = sched_add_proc(newPid, newBank, p);

                        // set zero pid retval for child
                        BANK_SEL = newBank;
                        sStruct = (struct ForkSyscallStruct *)(*((
                            size_t *)(newProcess->ap))); // syscall struct
                                                         // pointer sits in
                                                         // first arg in arg
                                                         // space
                        sStruct->pid = 0;

                        // set child pid as retval for parent
                        BANK_SEL = currentBank;
                        sStruct = (struct ForkSyscallStruct *)(*((
                            size_t *)(p->ap))); // syscall struct pointer sits
                                                // in first arg in arg space
                        sStruct->pid = newPid;

                        kernelTaskQueue[i].type = KERNEL_TASK_NONE;
                        ei();
                    } else {
                        printf("Kernel Worker: pid %d not found!\n",
                               kernelTaskQueue[i].src);
                    }
                }
            }
        }
    }
}

void addKernelTask(unsigned int task, unsigned int src, unsigned int dst) {
    int i = 0;
    for (i = 0; i < MAX_QUEUE_SIZE; i++) {
        if (kernelTaskQueue[i].type == KERNEL_TASK_NONE) {
            break;
        }
    }

    if (i == MAX_QUEUE_SIZE)
        return;

    kernelTaskQueue[i].type = task;
    kernelTaskQueue[i].src = src;
    kernelTaskQueue[i].dst = dst;
}
