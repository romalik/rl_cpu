#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <stdlib.h>

char cmdBuf[127];
int cmdBufSize = 127;
int cmdBufPos = 0;
char *nArgv[10];
int nArgc;
int bgChildren = 0;

extern char ** environ;

int cls(int argc, char **argv) {
  printf("%c[1J%c[H", 0x1b, 0x1b);
  return 0;
}

int cd(int argc, char **argv) {
  if (argc > 1) {
    int r = chdir(argv[1]);
    if (r) {
      printf("bad dir!\n");
    }
  }
  return 0;
}

void do_pwd() {
  char path[64*4];
  getcwd(path, 64*4);
  printf("%s", path);
}

int pwd(int argc, char ** argv) {
  do_pwd();
  printf("\n");
  return 0;
}

int exit_sh(int argc, char **argv) {
  exit(0);
  return 0;
}

void execWithPathSearch(char * nArgv[]);

int exec_sh(int argc, char **argv) {
  if(argc > 1) {
    execWithPathSearch(&argv[1]);
  }
  return 0;
}

int export_sh(int argc, char **argv) {
  if(argc > 1) {
    char *name;
    char *value;
    name = argv[1];
    value = argv[1];
    while(*value) {
      if(*value == '=') {
        *value = 0;
        value++;
        setenv(name,value,1);
        return 0;
      }
      value++;
    }
  }
  printf("bad format\n");
  return 0;
}

int help(int argc, char **argv);

char builtinCmds[][15] = {"cls", "cd", "help", "exit", "pwd", "export", "exec", ""};

int (*builtinFuncs[])(int argc, char **argv) = {cls, cd, help, exit_sh, pwd, export_sh, exec_sh};

int help(int argc, char **argv) {
  int i = 0;
  printf("Builtin commands:\n");

  while (builtinCmds[i][0] != 0) {
    printf("%s\n", builtinCmds[i]);
    i++;
  }
  return 0;
}

int addChar(char c) {
  if (c == '\n') {
    cmdBuf[cmdBufPos] = 0;
    cmdBufPos = 0;
    return 1;
  }
  if (cmdBufPos < cmdBufSize - 1) {
    cmdBuf[cmdBufPos] = c;
    cmdBufPos++;
  }
  return 0;
}

char * getToken(char * s, char ** tok) {
  int hasMore = 1;
  while(isspace(*s)) s++;
  *tok = s;
  while(!isspace(*s) && *s) s++;
  if(!*s) hasMore = 0;

  *s = 0;
  s += hasMore;
  return s;
}

/*
int sh_getArgs(char *cmd, char **_argv, int * redirIN, int * redirOUT, int * redirERR) {
    int argc = 0;
    char *s = cmd;
    _argv[argc] = s;
    argc++;
    while (*s) {
        if ((*s) == ' ') {
            *s = 0;
            _argv[argc] = s + 1;
            argc++;
        }
        s++;
    }
    _argv[argc] = 0;
    return argc;
}
*/

int sh_getArgs(char *cmd, char **_argv, int * redirIN, int * redirOUT, int * redirERR) {
  int argc = 0;
  char *s = cmd;

  while(1) {
    s = getToken(s, &(_argv[argc]));

    if(*_argv[argc] == '>') {
      int append = 0;
      if(*(_argv[argc]+1) == '>') {
        append = 1;
      }

      if(!*s) {
        printf("err\n");
        return 0;
      }
      s = getToken(s, &(_argv[argc]));
      if(append) {
        *redirOUT = open(_argv[argc], O_WRONLY|O_APPEND);
      } else {
        *redirOUT = open(_argv[argc], O_WRONLY|O_CREAT);
      }
    } else if(*_argv[argc] == '<') {
      if(!*s) {
        printf("err\n");
        return 0;
      }
      s = getToken(s, &(_argv[argc]));
      *redirIN = open(_argv[argc], O_RDONLY);

    } else {
      argc++;
    }

    if(!*s) break;
  }
  _argv[argc] = 0;
  return argc;
}

void execWithPathSearch(char * nArgv[]) {
  char * pathList;
  char * ep;
  char fullPath[128];
  int asis = 0;

  asis = !!(strchr(nArgv[0],'/'));
  
  if(asis) {
    execve((void *)nArgv[0], (void *)nArgv, 0);
  } else {
    pathList = getenv("PATH");
    if(!pathList) {
      printf("PATH not defined!\n");
      return;
    }
    ep = pathList;
    while(1) {
      while(*ep && *ep != ':') {
        ep++;
      }
      memcpy(fullPath, pathList, ep-pathList);
      fullPath[ep-pathList] = '/';
      fullPath[ep-pathList+1] = 0;
      strcat(fullPath, nArgv[0]);
      execve((void *)fullPath, (void *)nArgv, environ);
      if(!(*ep)) break;
      ep++;
      pathList = ep;
    }
  }


}


int main(int argc, char ** argv) {
  int i = 0;
  int script_mode = 0;
  int script_fd = 0;

  //asm("blink");


  while(1) {
    char a = 'A';

    write(1, &a, 1);//asm("blink");
    write(1, "Hi from userspace!\n", 19);//asm("blink");
  }
  if(argc > 1) {
    script_mode = 1;
    script_fd = open(argv[1], O_RDONLY);
    if(!script_fd) { return 0; }

  }

  setenv("PATH","/:/bin/",1);

  printf("\n");
  do_pwd();
  printf("# ");
  while (1) {
    char c;
    if(script_mode) {
      int a;
      if(!read(script_fd, &a, 1)) return 0;
      c = a;

    } else {
      c = getchar();
    }
    putchar(c);
    if (addChar(c)) {
      if (cmdBuf[0] && cmdBuf[0] != '#') {
        int redirIN = -2;
        int redirOUT = -2;
        int redirERR = -2;
        int pipeStart = -1;
        int p[2];
        for(i=0; i<strlen(cmdBuf); i++) {
          if(cmdBuf[i] == '|') {
            cmdBuf[i] = 0;
            printf("Got pipe: left [%s] right [%s]\n", cmdBuf, cmdBuf + i + 1);
            pipeStart = i + 1;

          }
        }

        nArgc = sh_getArgs(cmdBuf, nArgv, &redirIN, &redirOUT, &redirERR);

        if(redirIN == -1) goto err;
        if(redirOUT == -1) goto err;
        if(redirERR == -1) goto err;



        i = 0;
        while (builtinCmds[i][0] != 0) {
          if (!strcmp(nArgv[0], builtinCmds[i])) {
            int retval = builtinFuncs[i](nArgc, nArgv);
            break;
          }
          i++;
        }
        if (builtinCmds[i][0] == 0) {
          if(pipeStart < 0) {
            unsigned int childPid = fork();
            if (!childPid) {

              if(redirIN > 0) {
                close(STDIN_FILENO);
                dup(redirIN);
                close(redirIN);
              }
              if(redirOUT > 0) {
                close(STDOUT_FILENO);
                dup(redirOUT);
                close(redirOUT);
              }
              if(redirERR > 0) {
                close(STDERR_FILENO);
                dup(redirERR);
                close(redirERR);
              }

              if(nArgv[0][0] == '&') {
                //execve((void *)nArgv[1], (void *)(&nArgv[1]), 0);
                execWithPathSearch(&nArgv[1]);
                printf("Failed execing %s\n", nArgv[0]);
                return 1;
              } else {
                //execve((void *)nArgv[0], (void *)nArgv, 0);
                execWithPathSearch(nArgv);
                printf("Failed execing %s\n", nArgv[0]);
                return 1;
              }
            } else {
              int r = -1;
              int status;
              if(nArgv[0][0] == '&') {
                bgChildren++;
              } else {
                r = waitpid(childPid, &status, 0);
              }
            }
          } else {
            unsigned int pid1;
            unsigned int pid2;
            int r;
            int status;
            if(pipe(p) < 0) {
              //panic("pipe");
            }
            if((pid1 = fork()) == 0){
              close(1);
              dup(p[1]);
              close(p[0]);
              close(p[1]);
              //execve((void *)nArgv[0], (void *)nArgv, 0);
              execWithPathSearch(nArgv);
            }
            if((pid2 = fork()) == 0){
              nArgc = sh_getArgs(cmdBuf+pipeStart, nArgv, &redirIN, &redirOUT, &redirERR);
              close(0);
              dup(p[0]);
              close(p[0]);
              close(p[1]);
              //execve((void *)nArgv[0], (void *)nArgv, 0);
              execWithPathSearch(nArgv);
            }
            close(p[0]);
            close(p[1]);
            r = waitpid(pid1, &status, 0);
            r = waitpid(pid2, &status, 0);
            //break;
          }
        }
err:
        if(redirIN > 0) close(redirIN);
        if(redirOUT > 0) close(redirOUT);
        if(redirERR > 0) close(redirERR);
      }

      while(bgChildren) {
        int status;
        int rval;
        rval = waitpid(-1, &status, WNOHANG);
        if(rval) {
          bgChildren--;
          printf("[%d] done\n", rval, status);
        } else {
          break;
        }
      }
      do_pwd();
      printf("# ");
    }
  }
  return 0;
}
