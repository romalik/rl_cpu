#include <stdio.h>
#include <string.h>
#include <unistd.h>

char cmdBuf[127];
int cmdBufSize = 127;
int cmdBufPos = 0;
char *nArgv[10];
int nArgc;

int cls(int argc, char **argv) {
    printf("%c[1J%c[HThis should clear screen!\n", 0x1b, 0x1b);
    return 0;
}

int cd(int argc, char **argv) {
    /*
    if (argc > 1) {
        stat_t s;
        s = k_stat(argv[1]);
        if (s.flags == FS_DIR) {
            cProc->cwd = s.node;
        } else {
            printf("bad file");
        }
    }
    */
    return 0;
}

int exit_sh(int argc, char **argv) {
    exit(0);
    return 0;
}

int help(int argc, char **argv);

char builtinCmds[][15] = {"cls", "cd", "help", "exit", ""};

int (*builtinFuncs[])(int argc, char **argv) = {cls, cd, help, exit_sh};

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

int sh_getArgs(char *cmd, char **_argv) {
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

int main() {
    int i = 0;
    printf("User mode shell\n# ");
    while (1) {
        char c = getc();
        putc(c);
        if (addChar(c)) {
            if (cmdBuf[0]) {
                nArgc = sh_getArgs(cmdBuf, nArgv);
                i = 0;
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
                        execve((void *)nArgv[0], (void *)nArgv, 0);
                        printf("Failed execing %s\n", nArgv[0]);
                        return 1;
                    } else {
                        int r = -1;
                        r = waitpid(childPid);
                    }
                }
            }
            printf("# ");
        }
    }
    return 0;
}
