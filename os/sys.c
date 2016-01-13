#include <stdio.h>
#include <string.h>
#include "sh.h"
#include "ata.h"
#include "rlfs.h"
#include "malloc.h"
#include "types.h"
#include <memmap.h>
#include <syscall.h>


/* Syscalls:
 *  1 - put char
 *    [1][c]
 *  2 - get char
 *    [2][&c]
 *  3 - get ticks
 *    [3][&t]
 */

extern unsigned int ticks;




void system_interrupt(void * p) {
  int scall_id;
  scall_id = *(unsigned int *)p;
//  printf("KERNEL: syscall %d %c\n", scall_id, (*((unsigned int *)(p)+1)));

  if(scall_id == __NR_write) {
    kputc(*((unsigned int *)(p)+1));
  } else if(scall_id == __NR_read) {
    *((unsigned int *)(p)+1) = kgetc();
  } else if(scall_id == __NR_time) {
    *((unsigned int *)(p)+1) = ticks;
  } else {
    printf("Unknown syscall %d\n", scall_id);
  }



}

