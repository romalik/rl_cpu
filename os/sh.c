#include <stdio.h>
#include <string.h>
#include "sh.h"
#include "rlfs.h"


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

int rlfs_mkfs_main(char * args) {
  printf("Making filesystem\n");
  rlfs_mkfs();
  printf("done\n");
}

int rlfs_create_main(char * args) {
  printf("Creating file\n");
  rlfs_create("Testfile");

  printf("done\n");
}

int rlfs_write_main(char * args) {
  int fd;
  printf("Writing file\n");
  fd = rlfs_open("Testfile",'w');
  rlfs_write(fd, 'H');
  rlfs_write(fd, 'e');
  rlfs_write(fd, 'l');
  rlfs_write(fd, 'l');
  rlfs_write(fd, 'o');
  rlfs_write(fd, 0);

  rlfs_close(fd);

  printf("done\n");
}

int rlfs_read_main(char * args) {
  int fd;
  printf("Reading file\n");
  fd = rlfs_open("Testfile",'r');
  while(!rlfs_isEOF(fd)) {
    putc(rlfs_read(fd));
  }
  rlfs_close(fd);
  printf("\ndone\n");
}

int rlfs_size_main(char * args) {
  int fd;
  int size;
  printf("Size file\n");
  fd = rlfs_open("Testfile",'r');

  size = rlfs_tellg(fd);

  printf("Size :%d\n", size);
  rlfs_close(fd);
  printf("\ndone\n");
}

char builtinCmds[][10] = {
  "echo",
  "hello",
  "fs_mkfs",
  "fs_create",
  "fs_write",
  "fs_read",
  "fs_size",
  ""
};

int (*builtinFuncs[]) (char * args) = {
  echo_main,
  hello_main,
  rlfs_mkfs_main,
  rlfs_create_main,
  rlfs_write_main,
  rlfs_read_main,
  rlfs_size_main
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

