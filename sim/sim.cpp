#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>
unsigned int REG_A = 0;
unsigned int REG_B = 0;
unsigned int REG_C = 0;

unsigned int PC = 0;

unsigned int memoryMapSize;


unsigned long gettime_ms()
{
    struct timeval  tv;
    unsigned long stamp_ms;
    gettimeofday(&tv, NULL);
    stamp_ms = ((unsigned long)tv.tv_sec) * 1000 + (unsigned long)tv.tv_usec / 1000;
    return stamp_ms;
}

int kbhit(void)
{
  struct termios oldt, newt;
  int ch;
  int oldf;

  tcgetattr(STDIN_FILENO, &oldt);
  newt = oldt;
  newt.c_lflag &= ~(ICANON | ECHO);
  tcsetattr(STDIN_FILENO, TCSANOW, &newt);
  oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
  fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

  ch = getchar();

  tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
  fcntl(STDIN_FILENO, F_SETFL, oldf);

  if(ch != EOF)
  {
    ungetc(ch, stdin);
    return 1;
  }

  return 0;
}

typedef struct _memoryMapEntry {
    void (* writer)(unsigned int addr, unsigned int val, _memoryMapEntry * where);
    unsigned int (* reader)(unsigned int addr, _memoryMapEntry * where);
    unsigned int * storage;
    unsigned int blockStart;
    unsigned int blockSize;
    unsigned int writeProtected;
} MemoryMapEntry;

unsigned int ROM[8192];

unsigned int RAM_LOWER[16384];
unsigned int RAM_HIGHER[32768];

unsigned int kb_buf[255];
unsigned int kb_buf_idx = 0;

void writeMem(unsigned int addr, unsigned int val, MemoryMapEntry * where) {
    unsigned int localAddr = addr - where->blockStart;
    if(localAddr < where->blockSize)
        if(!where->writeProtected)
            where->storage[localAddr] = val;
        else
            printf("TRY WRITE WRITE-PROTECTED!\n");
    else
        printf("TRY WRITE OUT OF BOUNDS!\n");
}


unsigned int readMem(unsigned int addr, MemoryMapEntry * where) {
    unsigned int localAddr = addr - where->blockStart;
    if(localAddr < where->blockSize)
        return where->storage[localAddr];
    else
        printf("TRY READ OUT OF BOUNDS!\n");
    return 0xffff;
}


void portWriter(unsigned int addr, unsigned int val, MemoryMapEntry * where) {
    unsigned int localAddr = addr - where->blockStart;
    if(localAddr == 0) { //TTY port
        printf("%c", (char)(val & 0xff));
    }
}

unsigned int portReader(unsigned int addr, MemoryMapEntry * where) {
    unsigned int localAddr = addr - where->blockStart;
    if(localAddr == 1) { //KB port
        if(kb_buf_idx) {
            kb_buf_idx--;
            fprintf(stderr, "KB %04X idx %d\n", kb_buf[kb_buf_idx], kb_buf_idx);
            return kb_buf[kb_buf_idx];
        } else {
            fprintf(stderr, "KB ZERO\n");
            return 0;
        }
    }
}

MemoryMapEntry * memoryMap;

void w(unsigned int addr, unsigned int val) {
    for(int i = 0; i<memoryMapSize; i++) {
        if(addr >= memoryMap[i].blockStart && addr < memoryMap[i].blockStart + memoryMap[i].blockSize) {
            memoryMap[i].writer(addr, val, &memoryMap[i]);
        }
    }
}

unsigned int r(unsigned int addr) {
    for(int i = 0; i<memoryMapSize; i++) {
        if(addr >= memoryMap[i].blockStart && addr < memoryMap[i].blockStart + memoryMap[i].blockSize) {
            return memoryMap[i].reader(addr, &memoryMap[i]);
        }
    }
    return 0xffff;
}

void init() {
    memoryMapSize = 4;
    memoryMap = (MemoryMapEntry *)malloc(memoryMapSize*sizeof(MemoryMapEntry));

    memset(memoryMap, 0, memoryMapSize * sizeof(MemoryMapEntry));

    memoryMap[0].writer = &writeMem;
    memoryMap[0].reader = &readMem;
    memoryMap[0].storage = ROM;//(unsigned int *)malloc(8192*sizeof(unsigned int));
    memoryMap[0].blockStart = 0;
    memoryMap[0].blockSize = 8192;
    memoryMap[0].writeProtected = 0;

    memoryMap[1].writer = &portWriter;
    memoryMap[1].reader = &portReader;
    memoryMap[1].storage = 0;//(unsigned int *)malloc(8192*sizeof(unsigned int));
    memoryMap[1].blockStart = 8192;
    memoryMap[1].blockSize = 8192;
    memoryMap[1].writeProtected = 0;


    memoryMap[2].writer = &writeMem;
    memoryMap[2].reader = &readMem;
    memoryMap[2].storage = RAM_LOWER;//(unsigned int *)malloc(8192*sizeof(unsigned int));
    memoryMap[2].blockStart = 16384;
    memoryMap[2].blockSize = 16384;
    memoryMap[2].writeProtected = 0;

    memoryMap[3].writer = &writeMem;
    memoryMap[3].reader = &readMem;
    memoryMap[3].storage = RAM_HIGHER;//(unsigned int *)malloc(8192*sizeof(unsigned int));
    memoryMap[3].blockStart = 32768;
    memoryMap[3].blockSize = 32768;
    memoryMap[3].writeProtected = 0;

    REG_A = REG_B = REG_C = PC = 0;

    memset(ROM,0,8192*sizeof(unsigned int));
    memset(RAM_LOWER,0,16384*sizeof(unsigned int));
    memset(RAM_HIGHER,0,32768*sizeof(unsigned int));


}


void textHexToBin(char * text, unsigned int * target, unsigned int size) {
    char *s = text;
    unsigned int *t = target;
    char buf[100];
    buf[0] = 0;
    while(1) {
        if(!(*s)) {
            break;
        } else if(isalnum(*s)) {
            int len = strlen(buf);
            buf[len] = *s;
            buf[len+1] = 0;
        } else {
            if(strlen(buf)) {
                *t = strtol(buf, NULL, 16);
                t++;
                buf[0] = 0;
                if(t - target > size)
                    break;
            }
        }
        s++;
    }
}


void loadROMImage(char * path) {
    FILE * fd;
    fd = fopen(path, "r");
    int c = 0;
    char romtext[10000];
    char * romtextptr = romtext;
    int skipLine = 1;
    while((c = fgetc(fd))!=EOF) {
        if(c == '\n' || c == '\r')
            skipLine = 0;
        if(!skipLine) {
            * romtextptr = c;
            romtextptr++;
        }
    }
    *romtextptr = 0;
    textHexToBin(romtext, ROM, 8192);

}


/*
{fetch}
1       PcMux                           pc out
2       ALE; if not [IntProcess] PcIntClock; if [IRQ] goto INTERRUPT
3       RamCS & R/W Ram         ram out
4       ILE                                     IL in

{load}

4       RSR                                     [reset RegCnt]

NEXT_REG:
5       PcClk                           inc PC
5       LFRC_Reset                      LOAD_FROM_RC = 0

6       DC_Clk                          load DerefCnt

DEREF_LOOP:
7       if DerefCnt == 0 goto INC_REG [??AND RESET CURRENT REG IF NOT LFRC ??]

8       LOAD_FROM_RC? RC : PC out
9       ALE                     al in
10      RamCS & R/W Ram         ram out

11      RDE                             RA or RB or RC in

11      [LFRC_Set]                      |[LOAD_FROM_RC = 1]


12      DCDL                            |DerefCnt--
12      DC_Clk                          |


13 goto DEREF_LOOP

INC_REG:

14      RSI                                     RegCnt++

15      if RegCnt == 3 goto EXECUTE else goto NEXT_REG


EXECUTE:

16      RCMux;
        if(jmp_bit) {
          if(jmp_ok) {
            PcIncLoad = 0
          }
        } else {
          goto ALU
        }

17      PcClock & goto START

ALU:
18      ALE & PcClock
19      ALUMUX OR PcMux & RAMCS & goto START
INTERRUPT:

20      IntProcessStart                         //Merge 20 & 21  ???
21      IntVectorMux; PcLoad
22      PcClock; PcLoad; goto START;

*/


void execCycle() {
    //fetch
    unsigned int IR = r(PC);
    fprintf(stderr, "PC: %04X IR: %04X\n", PC, IR);

    //dereference
    int derefCnt = 0;
    int regCnt = 0;
    int loadFromRC = 0;

    while(regCnt < 3) {
        PC++;
        loadFromRC = 0;
        derefCnt = (IR & (0x3 << (5 + regCnt*2))) >> (5 + regCnt*2);
        while(derefCnt) {
            if(derefCnt == 1) {
                if(regCnt == 0) REG_A = r(loadFromRC?REG_C:PC);
                if(regCnt == 1) REG_B = r(loadFromRC?REG_C:PC);
                if(regCnt == 2) REG_C = r(loadFromRC?REG_C:PC);
            } else {
                REG_C = r(loadFromRC?REG_C:PC);
            }
            loadFromRC = 1;
            derefCnt--;

        }
        regCnt++;
    }
    PC++;

    fprintf(stderr, "REG_A: %04X REG_B: %04X REG_C: %04X\n", REG_A, REG_B, REG_C);
    //execute
    if(IR & (0x1 << 3)) { //jmp
        unsigned char cmpWord = ((REG_A == REG_B) << 2) | ((REG_A < REG_B) << 1) | ((REG_A > REG_B) << 0);
        fprintf(stderr, "JUMP cmpWord: %04X op %04X\n", cmpWord, (IR & 0x7));
        if(((IR & 0x7) & cmpWord) == 0) {
            //jmp ok
            fprintf(stderr, "JUMP TO %04X\n", REG_C);
            PC = REG_C;

        }
    } else {
        if(IR & (0x1 << 4)) {
            w(REG_C, PC);
        } else {
            fprintf(stderr, "ALU working\n");
            if((IR & 0x7) == 0) w(REG_C,REG_A << 1);
            if((IR & 0x7) == 1) w(REG_C,REG_A >> 1);
            if((IR & 0x7) == 2) w(REG_C,REG_A + REG_B);
            if((IR & 0x7) == 3) w(REG_C,REG_A - REG_B);
            if((IR & 0x7) == 4) w(REG_C,REG_A & REG_B);
            if((IR & 0x7) == 5) w(REG_C,REG_A | REG_B);
        }
    }

}

void checkKb() {
    int c = kbhit();
    if(c && kb_buf_idx < 255) {
        kb_buf[kb_buf_idx] = getchar();
        kb_buf_idx++;
    }
}

int main(int argc, char ** argv) {
    system("stty -echo");
    init();
    loadROMImage(argv[1]);
    while(1) {
        unsigned long t = gettime_ms();
        execCycle();
        checkKb();
        while(gettime_ms() - t < 1) {}
        unsigned long t2 = gettime_ms();

        fprintf(stderr, "cycle: %llu ms, freq: %f Hz\n", t2-t, 1000.0f/(t2-t));
    }



    return 0;
}
