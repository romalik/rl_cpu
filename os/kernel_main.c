#include <stdio.h>
#include <string.h>
#include "sh.h"
#include "ata.h"
#include "rlfs.h"
#include "malloc.h"
#include "types.h"
extern char  __data_end;
extern char  __code_end;

int main() {

  malloc_init((size_t)&__data_end, (size_t)(0x3000));

  printf("Init ata..\n");
  ataInit();
  rlfs_init();
  printf("Starting shell\n");
  main_sh();
  printf("System halted\n");
  while(1) {}
  return 0;
}
