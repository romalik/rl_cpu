#include <kstdio.h>
#include <string.h>
#include "sh_builtin.h"
#include "rlfs3.h"
#include "malloc.h"
#include <memmap.h>
#include <sched.h>
char cmdBuf[127];
int cmdBufSize = 127;
int cmdBufPos = 0;
char *nArgv[10];
int nArgc;

extern char __data_end;
extern char __code_end;

extern void syscall(void *p);

// extern unsigned int ticks;

int sync(int argc, char **argv) {
    block_sync();
    return 0;
}
int cls(int argc, char **argv) {
    printf("%c[1J%c[HThis should clear screen!\n", 0x1b, 0x1b);
    return 0;
}

int mknod(int argc, char **argv) {
    if (argc != 5) {
        printf("Bad format\n");
    } else {
        unsigned int type;
        unsigned int major;
        unsigned int minor;

        type = argv[2][0];
        major = atoi(argv[3]);
        minor = atoi(argv[4]);

        k_mknod(argv[1], type, major, minor);
    }
    return 0;
}

int testDev(int argc, char **argv) {
    if (argc < 2) {
        printf("Bad!\n");
    } else {
        FILE *fileDev = k_open(argv[1], 'w');
        if (fileDev) {
            k_write(fileDev,
                    (unsigned int *)"Test string to write on a device\n", 33);
            k_close(fileDev);
        }
    }
    return 0;
}

int fs_test(int argc, char **argv) {
    FILE *fd;
    unsigned int teststring[] = "This is a test string";
    unsigned int test2[40];
    stat_t s;

    printf("Write file\n");
    fs_init();
    fd = k_open("/test", 'w');
    k_write(fd, teststring, strlen(teststring) + 1);
    k_close(fd);

    printf("Stat file\n");
    s = k_stat("/test");
    printf("Size %d\n", s.size);

    printf("Read file\n");
    fd = k_open("/test", 'r');
    k_read(fd, test2, s.size);
    k_close(fd);

    printf("File : %s\n", test2);

    block_sync();
    return 0;
}

int mkdir(int argc, char **argv) {
    if (argc > 1) {
        k_mkdir(argv[1]);
    }
    return 0;
}

int ls(int argc, char **argv) {
    FILE *dir;
    if (argc > 1) {
        dir = k_opendir(argv[1]);
    } else {
        dir = k_opendir(".");
    }
    if (!dir) {
        printf("no dir\n");
        return 0;
    } else {
        while (1) {
            dirent_t dEnt;
            dEnt = k_readdir(dir);
            if (dEnt.idx == 0) {
                k_close(dir);
                return 0;
            }
            printf("%s\n", dEnt.name);
        }
    }
    return 0;
}

int cd(int argc, char **argv) {
    if (argc > 1) {
        stat_t s;
        s = k_stat(argv[1]);
        if (s.flags == FS_DIR) {
            cProc->cwd = s.node;
        } else {
            printf("bad file");
        }
    }
    return 0;
}

int loadBin(int argc, char **argv) {
    FILE *fd;
    size_t cPos;
    int bank;
    unsigned int read_size = 0x8000;
    bank = atoi(argv[2]);
    fd = k_open(argv[1], 'r');
    cPos = 0x8000;
    BANK_SEL = bank;

    if (fd->size < read_size)
        read_size = fd->size;

    //    while (!k_isEOF(fd)) {
    cPos += k_read(fd, (unsigned int *)cPos, read_size);
    //    }
    k_close(fd);
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
    FILE *fdIn;
    FILE *fdOut;
    unsigned int c;
    unsigned int cWord;
    unsigned int cnt;
    fdIn = k_open(argv[1], 'r');
    fdOut = k_open(argv[2], 'w');
    if (fdIn == NULL) {
        printf("file %s not found\n", argv[i]);
        return 1;
    }
    cWord = 0;
    i = 0;
    cnt = 0;
    while (!k_isEOF(fdIn)) {
        unsigned int v = 0;
        k_read(fdIn, &c, 1);
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
            k_write(fdOut, &cWord, 1);
            i = 0;
            cWord = 0;
            if ((cnt % 100) == 0) {
                printf("%u bytes written\n", cnt);
            }
        }
    }

    k_close(fdIn);
    k_close(fdOut);
    printf("\n");

    return 0;
}

int hexdump(int argc, char **argv) {
    /*
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
    */
    return 0;
}

int edit(int argc, char **argv) {
    int i;
    FILE *fd;
    int c;
    fd = k_open(argv[1], 'w');
    if (!fd) {
        printf("no file\n");
        return 0;
    }
    while (1) {
        c = getc();
        if (c == 0x04) {
            break;
        } else {
            putc(c);
            k_write(fd, (unsigned int *)&c, 1);
        }
    }
    k_close(fd);
    return 0;
}

int rm(int argc, char **argv) {
    /*
      int i;
        for (i = 1; i < argc; i++) {
            int r;
            r = rlfs_removeFile(argv[i]);
            if (r < 0) {
                printf("File not found: %s!\n", argv[i]);
            }
        }
    */
    return 0;
}

int cat(int argc, char **argv) {
    int i = 0;
    if (argc == 1) {
    } else {
        for (i = 1; i < argc; i++) {
            FILE *fd = k_open(argv[i], 'r');
            if (!fd) {
                printf("File [%s] not found!\n", argv[i]);
            } else {
                while (!k_isEOF(fd)) {
                    unsigned int val = 0;
                    k_read(fd, &val, 1);
                    putc(val);
                }
                k_close(fd);
            }
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
    fs_mkfs();
    printf("done\n");
}

int help(int argc, char **argv);

char builtinCmds[][15] = {"cls",     "mknod",   "testDev", "sync",    "cd",
                          "mkdir",   "fs_test", "loadBin", "runBin",  "help",
                          "hex2bin", "uptime",  "usemem",  "meminfo", "hexdump",
                          "edit",    "rm",      "cat",     "ls",      "echo",
                          "hello",   "fs_mkfs", ""};

int (*builtinFuncs[])(int argc, char **argv) = {
    cls,     mknod,     testDev,    sync,          cd,      mkdir,
    fs_test, loadBin,   runBin,     help,          hex2bin, uptime,
    usemem,  meminfo,   hexdump,    edit,          rm,      cat,
    ls,      echo_main, hello_main, rlfs_mkfs_main};

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
