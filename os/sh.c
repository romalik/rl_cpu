#include <stdio.h>
#include <string.h>
#include "sh.h"
#include "rlfs.h"
#include "heap.h"

char cmdBuf[127];
int cmdBufSize = 127;
int cmdBufPos = 0;
char * nArgv[10];
int nArgc;

extern char  __data_end;
extern char  __code_end;

int meminfo(int argc, char ** argv) {
    unsigned int *p1, *p2, *p3;
    printf("__data_end 0x%04X\n__code_end 0x%04X\n", &__data_end, &__code_end);

    p1 = (unsigned int *)malloc(0x100);
    p2 = (unsigned int *)malloc(0x200);
    p3 = (unsigned int *)malloc(0x300);
    
    printf("p1 0x%04x, p2 0x%04x, p3 0x%04x\n", p1, p2, p3);



    return 0;

}
int keyscan(int srgc, char ** argv) {
    while(1) {
        int c;
        c = getc();
        if(c == 0x04) {
            break;
        }
        printf("key pressed: 0x%04x, [%c]\n", c, c);
    }
    return 0;
}

int hexdump(int argc, char ** argv) {
    int i;
    for(i = 1; i<argc; i++) {
        int fd = rlfs_open(argv[i],'r');
        int c;
        if(fd < 0) {
            printf("file %s not found\n", argv[i]);
            continue;
        }
        while(!rlfs_isEOF(fd)) {
            c = rlfs_read(fd);
            printf("0x%04x ", c);
        }
    }
    printf("\n");

    return 0;
}

int memdump(int argc, char **argv) {
    unsigned int * c;
    unsigned int * end;
    if(argc < 3) {
        printf("usage: %s addr_begin addr_end\n", argv[0]);
        return -1;
    }
    c = (unsigned int *)atoi(argv[1]);
    end = (unsigned int *)atoi(argv[2]);
    for( ; c<end; c++) {
        printf("0x%04x ", *c);
    }
    printf("\n");
    return 0;
}

int edit(int argc, char ** argv) {
    int i;
    int fd;
    int c;
    fd = rlfs_open(argv[1],'w');
    while(1) {
        c = getc();
        if(c == 0x04) {
            break;
        } else {
            putc(c);
            rlfs_write(fd,c);
        }
    }
    rlfs_close(fd);
    
    return 0;
}


int rm(int argc, char ** argv) {
    int i;
    for(i = 1; i<argc; i++) {
        int r;
        r = rlfs_removeFile(argv[i]);
        if(r < 0) {
            printf("File not found: %s!\n", argv[i]);
        }
    }
    return 0;
}

int cat(int argc, char ** argv) {
    int i = 0;
    if(argc == 1) {

    } else {
        for(i = 1; i<argc; i++) {
            int fd = rlfs_open(argv[i],'r');
            if(fd < 0) {
                printf("File [%s] not found!\n", argv[i]); 
            } else {
                while(!rlfs_isEOF(fd)) {
                    putc(rlfs_read(fd));
                }
                rlfs_close(fd);
            }
        }
    }
    return 0;
}

int ls(int argc, char ** argv) {
    int i = 0;
    unsigned int buf[64*4];
    ataReadSectorsLBA(0,(unsigned char*)buf);
    for(i = 0; i < 256; i+=16) {
        if(buf[i] == 0) {
            break;
        } else if(buf[i] == 0xffff) {
            continue;
        } else {
            printf("%06d : %s\n", buf[i+1], (buf + i+3));    
        }
    }
    return 0;
}


int echo_main(int argc, char ** argv) {
/*
    unsigned long i;
    unsigned long j;
    int d;

    d = 100;

    i = 100000;
    j = 200000;

    if(d == 0xffff) {
        printf("Eq!\n");
    } else {
        printf("Ne!\n");
    }
*/

    int i = 0;

    for(i = 0; i<argc; i++) {
        printf("%d: %s\n", i, argv[i]);
    }

    return 0;
}

int hello_main(int argc, char ** argv) {
  int i;
  printf("Hello!\n");
  return 5;
}

int load_main(int argc, char ** argv) {
}

int rlfs_mkfs_main(int argc, char ** argv) {
  printf("Making filesystem\n");
  rlfs_mkfs();
  printf("done\n");
}

int rlfs_create_main(int argc, char ** argv) {
  printf("Creating file\n");
  rlfs_create("Testfile");

  printf("done\n");
}

int rlfs_write_main(int argc, char ** argv) {
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

int rlfs_read_main(int argc, char ** argv) {
  int fd;
  printf("Reading file\n");
  fd = rlfs_open("Testfile",'r');
  while(!rlfs_isEOF(fd)) {
    putc(rlfs_read(fd));
  }
  rlfs_close(fd);
  printf("\ndone\n");
}

int rlfs_size_main(int argc, char ** argv) {
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
  "meminfo",
  "memdump",
  "keyscan",
  "hexdump",
  "edit",
  "rm",
  "cat",
  "ls",
  "echo",
  "hello",
  "fs_mkfs",
  "fs_create",
  "fs_write",
  "fs_read",
  "fs_size",
  ""
};

int (*builtinFuncs[]) (int argc, char ** argv) = {
  meminfo,
  memdump,
  keyscan,
  hexdump,
  edit,
  rm,
  cat,
  ls,
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

int sh_getArgs(char * cmd, char ** _argv) {
    int argc = 0;
    char * s = cmd;
    _argv[argc] = s;
    argc++;
    while(*s) {
        if((*s)==' ') {
            *s = 0;
            _argv[argc] = s+1; 
            argc++;
        }
        s++;
    }
    return argc;
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

      nArgc = sh_getArgs(cmdBuf, nArgv);

      i = 0;
      while(builtinCmds[i][0] != 0) {
        if(!strcmp(nArgv[0],builtinCmds[i])) {
          int retval = builtinFuncs[i](nArgc, nArgv);
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

