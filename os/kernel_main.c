#include <stdio.h>
#include <string.h>
#include "sh.h"
#include "ata.h"
#include "rlfs.h"
#include "heap.h"

extern char  __data_end;
extern char  __code_end;

int main() {

//  if(__data_end > __code_end) {
     malloc_init(&__data_end, (unsigned int)(0x3000) - (unsigned int)(&__data_end));
//  } else {
//     malloc_init(&__code_end, (unsigned int)(0x4000) - (unsigned int)(&__code_end));
//  }

  printf("Init ata..\n");
  ataInit();
  rlfs_init();
  printf("Starting shell\n");
  main_sh();
  printf("System halted\n");
  while(1) {}
  return 0;
}
