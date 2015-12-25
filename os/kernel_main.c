#include <stdio.h>
#include <string.h>
#include "sh.h"
#include "ata.h"


int main() {


  unsigned char buf[64*4] = "this is a test string";
  printf("Init ata..\n");
  ataInit();
  ataWriteSectorsLBA(0, buf);

  printf("Starting shell\n");
  main_sh();
  printf("System halted\n");
  while(1) {}
  return 0;
}
