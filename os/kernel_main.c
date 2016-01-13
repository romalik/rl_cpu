#include <stdio.h>
#include <string.h>
#include "sh.h"
#include "ata.h"
#include "rlfs.h"
#include "malloc.h"
#include "types.h"

extern char  __data_end;
extern char  __code_end;
extern void __timer_interrupt_vector();
extern void __system_interrupt_vector();
unsigned int ticks = 0;

unsigned int sched_stack[2*64];
unsigned sched_active = 0;

extern void syscall();

#define PROC_STATE_NONE 0
#define PROC_STATE_RUN 1
#define PROC_STATE_WAIT 2
#define PROC_STATE_EXIT 3

struct Process {
  unsigned int pid;
  unsigned int state;
  unsigned int ap;
  unsigned int bp;
  unsigned int sp;
  unsigned int pc;

  unsigned int memBank;
};

#define MAXPROC 15

struct Process procs[MAXPROC];
unsigned int currentTask = 0;

void sched_init() {
  int i = 0;
  for(i = 0; i<MAXPROC; i++) {
    procs[i].state = PROC_STATE_NONE;
  }
  procs[0].state = PROC_STATE_RUN;

}

void sched_add_proc(unsigned int pid, unsigned int bank) {
  int i = 0;
  for(i = 0; i<MAXPROC; i++) {
    if(procs[i].state == PROC_STATE_NONE) {
      break;
    }
  }

  if(i == MAXPROC)
    return;

  procs[i].pid = pid;
  procs[i].memBank = bank;
  procs[i].ap = 0xC000;
  procs[i].bp = 0xC000;
  procs[i].sp = 0xC000;
  procs[i].pc = 0x8000;
  procs[i].state = PROC_STATE_RUN;

}

void sched_start() {
  currentTask = 0;
  sched_active = 1;
}

void timer_interrupt(unsigned int * ap, unsigned int * bp, unsigned int * pc, unsigned int * sp) {
  int nextTask;
  ticks++;

  if(sched_active) {
    nextTask = currentTask + 1;
    while(nextTask != currentTask) {
      if(nextTask == MAXPROC)
        nextTask = 0;

      if(procs[nextTask].state == PROC_STATE_RUN)
        break;

      nextTask++;

    }

    procs[currentTask].ap = *ap;
    procs[currentTask].bp = *bp;
    procs[currentTask].sp = *sp;
    procs[currentTask].pc = *pc;

    *ap = procs[nextTask].ap;
    *bp = procs[nextTask].bp;
    *sp = procs[nextTask].sp;
    *pc = procs[nextTask].pc;
    BANK_SEL = procs[nextTask].memBank;
    currentTask = nextTask;
  }
}

#define TIMER_INTERRUPT_ADDR_PORT INT3_vec
#define SYSTEM_INTERRUPT_ADDR_PORT INT0_vec

void init_interrupts() {
  TIMER_INTERRUPT_ADDR_PORT = (size_t)(__timer_interrupt_vector);
  SYSTEM_INTERRUPT_ADDR_PORT = (size_t)(__system_interrupt_vector);
  ei();
}



#define KERNEL_TASK_NONE 0
#define KERNEL_TASK_FORK 1
#define KERNEL_TASK_EXEC 2

struct KernelTask {
  unsigned int type;
  unsigned int src;
  unsigned int dst;
};


#define MAX_QUEUE_SIZE 15

struct KernelTask kernelTaskQueue[MAX_QUEUE_SIZE];

void kernel_worker_init() {
  int i = 0;
  for(i = 0; i<MAX_QUEUE_SIZE; i++) {
    kernelTaskQueue[i].type = KERNEL_TASK_NONE;
  }

}

void kernel_worker() {
  while(1) {
    int i = 0;
    for(i = 0; i<MAX_QUEUE_SIZE; i++) {
      if(kernelTaskQueue[i].type != KERNEL_TASK_NONE) {

      }
    }
    printf("kworker online!\n");
  }
}

void addKernelTask(unsigned int task, unsigned int src, unsigned int dst) {
  int i = 0;
  for(i = 0; i<MAX_QUEUE_SIZE; i++) {
    if(kernelTaskQueue[i].type == KERNEL_TASK_NONE) {
      break;
    }
  }

  if(i == MAX_QUEUE_SIZE)
    return;

  kernelTaskQueue[i].type = task;
  kernelTaskQueue[i].src = src;
  kernelTaskQueue[i].dst = dst;


}

int kernel_main() {

  malloc_init((size_t)&__data_end, (size_t)(0x3000));
//  printf("Init interrupts..\n");
  init_interrupts();

  printf("Init ata..\n");
  ataInit();
  printf("Init fs..\n");
  rlfs_init();
  printf("Press s for shell, any key for init\n");
  if(kgetc() == 's') {
    main_sh();
  } else {
    int fd1 = rlfs_open("task1.bin", 'r');
    int fd2 = rlfs_open("task2.bin", 'r');

    size_t cPos = 0x8000;
    BANK_SEL = 0;
    printf("load task1\n");
    while(!rlfs_isEOF(fd1)) {
      *(unsigned int *)(cPos) = rlfs_read(fd1);
      cPos++;
    }
    rlfs_close(fd1);

    cPos = 0x8000;
    BANK_SEL = 1;
    printf("load task2\n");
    while(!rlfs_isEOF(fd2)) {
      *(unsigned int *)(cPos) = rlfs_read(fd2);
      cPos++;
    }
    rlfs_close(fd2);

    sched_init();
    sched_add_proc(1,0);
    sched_add_proc(2,1);

    printf("Starting scheduler\n");
    sched_start();
    kernel_worker();
  }
  printf("System halted\n");
  while(1) {}
  return 0;
}
