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

extern void syscall();

void timer_interrupt() {
  ticks++;
}

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
  rlfs_init();
  printf("Starting shell\n");
  main_sh();
  printf("System halted\n");
  while(1) {}
  return 0;
}
