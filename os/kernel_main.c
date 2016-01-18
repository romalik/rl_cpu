#include <stdio.h>
#include <string.h>
#include "sh.h"
#include "ata.h"
#include "rlfs.h"
#include "malloc.h"
#include "types.h"
#include "sched.h"
#include "kernel_worker.h"
#include <mm.h>
#include <vfs.h>
#include <blk.h>

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




int kernel_main() {

  malloc_init((size_t)&__data_end, (size_t)(0x3000));
//  printf("Init interrupts..\n");
  init_interrupts();

  printf("Init ata..\n");
  ataInit();
  printf("Init fs..\n");
  block_init();
  vfs_init();
  rlfs_init();
  mm_init();

  printf("Press s for shell, any key for init\n");

  if(kgetc() == 's') {
    main_sh();
  } else {
    unsigned int b;
    int fd1 = rlfs_open("forkbomb.bin", 'r');
    size_t cPos = 0x8000;
    mm_allocSegment(&b);
    BANK_SEL = b;
    printf("load task1\n");
    while(!rlfs_isEOF(fd1)) {
      *(unsigned int *)(cPos) = rlfs_read(fd1);
      cPos++;
    }
    rlfs_close(fd1);

    sched_init();

    sched_add_proc(sched_genPid(),b,0);

    printf("Starting scheduler\n");
    sched_start();
    kernel_worker();
  }
  printf("System halted\n");
  while(1) {}
  return 0;
}
