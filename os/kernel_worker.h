#ifndef KERNEL_WORKER__
#define KERNEL_WORKER__
#include <syscall.h>
#include <sched.h>

#define KERNEL_TASK_NONE 0
#define KERNEL_TASK_FORK 1
#define KERNEL_TASK_EXEC 2

struct KernelTask {
    unsigned int type;
    unsigned int src;
    unsigned int dst;
};

#define MAX_QUEUE_SIZE 15

void kernel_worker_init();
void kernel_worker();
void addKernelTask(unsigned int task, unsigned int src, unsigned int dst);

#endif
