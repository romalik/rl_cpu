#include <stdio.h>
#include <string.h>
#include "sh.h"
#include "ata.h"
#include "rlfs.h"
#include "malloc.h"
#include "types.h"
#include "sched.h"


extern char  __data_end;
extern char  __code_end;
extern void __timer_interrupt_vector();
extern void __system_interrupt_vector();

extern void syscall();

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
