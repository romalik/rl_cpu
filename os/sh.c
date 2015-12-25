#include <stdio.h>
#include <string.h>
#include "sh.h"

char cmdBuf[127];
int cmdBufSize = 127;
int cmdBufPos = 0;


int echo_main(char * args) {
  return 0;
}

int hello_main(char * args) {
  printf("Hello!\n");
  return 5;
}

int load_main(char * args) {
}


char builtinCmds[][10] = {
  "echo",
  "hello",
  "load",
  ""
};

int (*builtinFuncs[]) (char * args) = {
  echo_main,
  &hello_main,
  load_main
};


int addChar(char c) {
  if(c == '\n') {
    cmdBuf[cmdBufPos] = 0;
    cmdBufPos = 0;
    return 1;
  }
  if(cmdBufPos < cmdBufSize-1) {
    cmdBuf[cmdBufPos] = c;
    cmdBufPos++;
  }
  return 0;
}



int main_sh() {
  int i = 0;
  printf("Builtin commands:\n");
  while(builtinCmds[i][0] != 0) {
    printf("%s : %04x\n", builtinCmds[i], (int)builtinFuncs[i]);
    i++;
  }
  printf("# ");
  while(1) {
    char c = getc();
    putc(c);
    if(addChar(c)) {
      printf("Cmd entered: %s\n", cmdBuf);
      i = 0;
      while(builtinCmds[i][0] != 0) {
        if(!strcmp(cmdBuf,builtinCmds[i])) {
          int retval = builtinFuncs[i](cmdBuf);
          printf("Exitcode: %d\n",retval);
          break;
        }
        i++;
      }
      printf("# ");
    }
  }
  return 0;
}

