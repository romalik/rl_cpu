#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

char cmdBuf[127];
int cmdBufSize = 127;
int cmdBufPos = 0;
char *nArgv[10];
int nArgc;
int bgChildren = 0;


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

int help(int argc, char **argv);

char builtinCmds[][15] = {"cls", "cd", "help", "exit", "pwd", ""};

int (*builtinFuncs[])(int argc, char **argv) = {cls, cd, help, exit_sh, pwd};

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


int main() {
    int i = 0;
    printf("\n");
    do_pwd();
    printf("# ");
    while (1) {
        char c = getchar();
        putchar(c);
        if (addChar(c)) {
            if (cmdBuf[0]) {
                int redirIN = -2;
                int redirOUT = -2;
                int redirERR = -2;
                nArgc = sh_getArgs(cmdBuf, nArgv, &redirIN, &redirOUT, &redirERR);
                i = 0;

                if(redirIN == -1) goto err;
                if(redirOUT == -1) goto err;
                if(redirERR == -1) goto err;




                while (builtinCmds[i][0] != 0) {
                    if (!strcmp(nArgv[0], builtinCmds[i])) {
                        int retval = builtinFuncs[i](nArgc, nArgv);
                        break;
                    }
                    i++;
                }
                if (builtinCmds[i][0] == 0) {
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
                            execve((void *)nArgv[1], (void *)(&nArgv[1]), 0);
                            printf("Failed execing %s\n", nArgv[0]);
                            return 1;
                        } else {
                            execve((void *)nArgv[0], (void *)nArgv, 0);
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
