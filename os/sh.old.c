#include <stdio.h>
#include <string.h>
#include "sh.h"
#include "rlfs.h"
#include "malloc.h"
#include <memmap.h>

char cmdBuf[127];
int cmdBufSize = 127;
int cmdBufPos = 0;
char *nArgv[10];
int nArgc;

extern char __data_end;
extern char __code_end;

extern void syscall(void *p);

// extern unsigned int ticks;

int loadBin(int argc, char **argv) {
    int fd;
    size_t cPos;
    int bank;
    bank = atoi(argv[2]);
    fd = rlfs_open(argv[1], 'r');
    cPos = 0x8000;
    BANK_SEL = bank;
    while (!rlfs_isEOF(fd)) {
        *(unsigned int *)(cPos) = rlfs_read(fd);
        cPos++;
    }
    rlfs_close(fd);
    printf("Done.\n");
    printf("%d words loaded\n", cPos - 0x8000);
    return 0;
}

extern int runBin(int argc, char **argv);

int uptime(int argc, char **argv) {
    printf("%u ticks\n", gettime());
    return 0;
}

int meminfo(int argc, char **argv) {
    unsigned int *p1, *p2, *p3, *p4;
    size_t sz;
    printf("__data_end 0x%04X\n__code_end 0x%04X\n", &__data_end, &__code_end);

    p1 = (unsigned int *)malloc(0x100);
    p2 = (unsigned int *)malloc(0x200);
    p3 = (unsigned int *)malloc(0x300);
    printf("p1 0x%04x, p2 0x%04x, p3 0x%04x\n", p1, p2, p3);

    free(p2);

    p4 = (unsigned int *)malloc(0x150);
    printf("p4 0x%04x\n", p4);

    free(p1);
    free(p3);
    free(p4);

    for (sz = 0xffff; sz > 0; sz--) {
        p1 = (unsigned int *)malloc(sz);
        if (p1)
            break;
    }
    if (sz && p1) {
        printf("Free mem: %u words\n", sz);
        free(p1);
    }
    return 0;
}

int usemem(int argc, char **argv) {
    size_t sz;
    unsigned int *p;
    printf("trying atoi %s\n", argv[1]);
    sz = atoi(argv[1]);

    printf("Allocating %u words..\n", sz);
    p = (unsigned int *)malloc(sz);
    if (p) {
        printf("Successful! p = 0x%04x\n", p);
    } else {
        printf("Failure!\n");
    }
    return 0;
}

int hex2bin(int argc, char **argv) {
    int i;
    int fdIn;
    int fdOut;
    int c;
    unsigned int cWord;
    unsigned int cnt;
    fdIn = rlfs_open(argv[1], 'r');
    fdOut = rlfs_open(argv[2], 'w');
    if (fdIn < 0) {
        printf("file %s not found\n", argv[i]);
        return 1;
    }
    cWord = 0;
    i = 0;
    cnt = 0;
    while (!rlfs_isEOF(fdIn)) {
        unsigned int v = 0;
        c = rlfs_read(fdIn);
        if (c >= 'a' && c <= 'f') {
            v = 10 + (c - 'a');
        } else if (c >= 'A' && c <= 'F') {
            v = 10 + (c - 'A');
        } else if (c >= '0' && c <= '9') {
            v = c - '0';
        } else {
            continue;
        }
        cWord = cWord + (v << ((3 - i) * 4));
        i++;
        if (i == 4) {
            cnt++;
            //            printf("0x%04x ", cWord);
            rlfs_write(fdOut, cWord);
            i = 0;
            cWord = 0;
            if ((cnt % 100) == 0) {
                printf("%u bytes written\n", cnt);
            }
        }
    }

    rlfs_close(fdIn);
    rlfs_close(fdOut);
    printf("\n");

    return 0;
}

int hexdump(int argc, char **argv) {
    int i;
    for (i = 1; i < argc; i++) {
        int fd = rlfs_open(argv[i], 'r');
        int c;
        if (fd < 0) {
            printf("file %s not found\n", argv[i]);
            continue;
        }
        while (!rlfs_isEOF(fd)) {
            c = rlfs_read(fd);
            printf("0x%04x ", c);
        }
    }
    printf("\n");

    return 0;
}

int edit(int argc, char **argv) {
    int i;
    int fd;
    int c;
    fd = rlfs_open(argv[1], 'w');
    while (1) {
        c = getc();
        if (c == 0x04) {
            break;
        } else {
            putc(c);
            rlfs_write(fd, c);
        }
    }
    rlfs_close(fd);

    return 0;
}

int rm(int argc, char **argv) {
    int i;
    for (i = 1; i < argc; i++) {
        int r;
        r = rlfs_removeFile(argv[i]);
        if (r < 0) {
            printf("File not found: %s!\n", argv[i]);
        }
    }
    return 0;
}

int cat(int argc, char **argv) {
    int i = 0;
    if (argc == 1) {
    } else {
        for (i = 1; i < argc; i++) {
            int fd = rlfs_open(argv[i], 'r');
            if (fd < 0) {
                printf("File [%s] not found!\n", argv[i]);
            } else {
                while (!rlfs_isEOF(fd)) {
                    putc(rlfs_read(fd));
                }
                rlfs_close(fd);
            }
        }
    }
    return 0;
}

int ls(int argc, char **argv) {
    int i = 0;
    unsigned int buf[64 * 4];
    ataReadSectorsLBA(0, buf);
    for (i = 0; i < 256; i += 16) {
        if (buf[i] == 0) {
            break;
        } else if (buf[i] == 0xffff) {
            continue;
        } else {
            printf("%06d : %s\n", buf[i + 1], (buf + i + 3));
        }
    }
    return 0;
}

int echo_main(int argc, char **argv) {
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

    for (i = 0; i < argc; i++) {
        printf("%d: %s\n", i, argv[i]);
    }

    return 0;
}

int hello_main(int argc, char **argv) {
    int i;
    printf("Hello!\n");
    return 5;
}

int load_main(int argc, char **argv) {
}

int rlfs_mkfs_main(int argc, char **argv) {
    printf("Making filesystem\n");
    rlfs_mkfs();
    printf("done\n");
}

int help(int argc, char **argv);

char builtinCmds[][15] = {"loadBin", "runBin", "help",    "hex2bin",
                          "uptime",  "usemem", "meminfo", "hexdump",
                          "edit",    "rm",     "cat",     "ls",
                          "echo",    "hello",  "fs_mkfs", ""};

int (*builtinFuncs[])(int argc, char **argv) = {
    loadBin, runBin,  help,      hex2bin,    uptime,
    usemem,  meminfo, hexdump,   edit,       rm,
    cat,     ls,      echo_main, hello_main, rlfs_mkfs_main};

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
    return argc;
}

int main_sh() {
    int i = 0;
    /*
      printf("Builtin commands:\n");

      while(builtinCmds[i][0] != 0) {
        printf("%s : %04x\n", builtinCmds[i], (int)builtinFuncs[i]);
        i++;
      }
      */
    printf("# ");
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
                    printf("Bad command\n");
                }
            }
            printf("# ");
        }
    }
    return 0;
}
