#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#include <ctype.h>
#include <unistd.h>

char text[1000000];
char text2[1000000];

typedef struct opcode_entry_ {
    char name[5];
    unsigned int code;
    unsigned int argC;
} opcode_entry;


/*
instruction
|  15  |  14  |  13  |  12  |  11  |  10  |  9  |  8  |  7  |  6  |  5  |  4   |   3   |  2  |  1  |  0  |
|  NA  |  NA  |  NA  |  NA  |  NA  |   deref2   |   deref1  |   deref0  |  LPC |alu/jmp|     opcode      |

alu:

add	2	0010
sub	3	0011
and	4	0100
or	5	0101

jmp:
        j=<>
jmp	8	1000
jle	9	1001
jge	a	1010
je	b	1011
jne	c	1100
jl	d	1101
jg	e	1110

hlt 0x8000 1000 0000 0000 0000
*/


opcode_entry opcode_table[] = {
    {"shl", 0x00, 4},
    {"shr", 0x01, 4},
    {"add", 0x02, 4},
    {"sub", 0x03, 4},
    {"and", 0x04, 4},
    {"or", 0x05, 4},
    {"jmp", 0x08, 4},
    {"jle", 0x09, 4},
    {"jge", 0x0a, 4},
    {"je", 0x0b, 4},
    {"jne", 0x0c, 4},
    {"jl", 0x0d, 4},
    {"jg", 0x0e, 4},
    {"lpc", 0x10, 4},
    {"reti", 0x808, 4},
    {"hlt", 0x8000, 4},
    {"", 0x00, 0}
};

unsigned int segmentOffset[2];

unsigned int globalOffset = 0;




typedef struct label_entry_ {
    char name[255];
    unsigned int addr;
    int segment;
} label_entry;

label_entry label_array[255];

void initLabelArray() {
    for(int i = 0; i<255; i++) {
        *label_array[i].name = 0;
        label_array[i].addr = 0;
        label_array[i].segment = 0;
    }
}

void addLabel(char * _name, unsigned int _addr, int segment) {
    for(int i = 0; i<255; i++) {
        if(!(*label_array[i].name)) {
            strcpy(label_array[i].name, _name);
            label_array[i].addr = _addr;
            label_array[i].segment = segment;
            return;
        }
    }
}

void loadLabelArray(char * fname) {
    FILE * fd = fopen(fname, "r");
    char lname[100];
    unsigned int laddr;
    while(fscanf(fd,"%s %x\n", lname, &laddr) != EOF) {
        addLabel(lname, laddr, -1);
    }
}

unsigned int getLabelAddress(char * _name) {
    for(int i = 0; i<255; i++) {
        if(!strcmp(_name, label_array[i].name)) {
            return label_array[i].addr;
        }
    }
    return 0;
}

unsigned int getLabelAbsAddress(char * _name) {
    for(int i = 0; i<255; i++) {
        if(!strcmp(_name, label_array[i].name)) {
            return label_array[i].addr + ((label_array[i].segment==-1)?0:(segmentOffset[label_array[i].segment] + globalOffset));
        }
    }
    return 0;
}


int getOpcodeTableIndex(char * token) {
    int cnt = 0;
    while(1) {
        if(*opcode_table[cnt].name == 0 || !*token) {
            cnt = -1;
            break;
        }
        if(!strcmp(opcode_table[cnt].name, token)) {
            //printf("> %s -> [0x%04X]\n", opcode_table[cnt].name, opcode_table[cnt].code);
            break;
        }
        cnt++;
    }
    return cnt;

}

int flSim = 0;

unsigned int memImage[0x10000];
unsigned int * seg[2];

int main(int argc, char ** argv) {

    char * outName = 0;
    char * outLinkName = 0;
    char * inLinkName = 0;
    char * inName = 0;
    int verbose = 0;


    opterr = 0;
    int c;
    while ((c = getopt (argc, argv, "s:O:l:L:o:v::")) != -1) {
        switch (c)
        {
        case 'v':
            verbose = 1;
            if(optarg)
                if(*optarg == 'v')
                    verbose = 2;
            break;
        case 'l':
            inLinkName = optarg;
            break;
        case 'L':
            outLinkName = optarg;
            break;
        case 'o':
            outName = optarg;
            break;
        case 's':
            inName = optarg;
            break;
        case 'O':
            globalOffset = strtol(optarg, 0, 0);
            break;
        //default:
            //abort ();
        }
    }
    if(verbose) {
        printf("Verbose: %d\noutName: %s\ninLinkName: %s\noutLinkName: %s\nglobalOffset: %d\ninName: %s\n", verbose, outName?outName:"NO", inLinkName?inLinkName:"NO", outLinkName?outLinkName:"NO", globalOffset, inName?inName:"NO");
    }


    memset(memImage,0,sizeof(unsigned int) * 0x10000);

    segmentOffset[0] = 0;
    segmentOffset[1] = 0x1020;
    seg[0] = (unsigned int *)malloc(0x10000 * sizeof(unsigned int));
    seg[1] = (unsigned int *)malloc(0x10000 * sizeof(unsigned int));
    memset(seg[0], 0, 0x10000*sizeof(unsigned int));
    memset(seg[1], 0, 0x10000*sizeof(unsigned int));
    if(argc > 1) {
        FILE * fd;
        fd = fopen(inName, "r");
        char * s = text;
        while(1) {
            int c;
            if((c = fgetc(fd)) == EOF)
                break;

            *s = (char)c;
            s++;

        }

        if(verbose) {
            printf("Global offset: 0x%04X\n", globalOffset);
            if(verbose == 2) printf("text:\n%s\n", text);
        }

        initLabelArray();
        if(inLinkName) {
            if(verbose) {
                printf("Loading link info from %s\n", inLinkName);
            }
            loadLabelArray(inLinkName);


        }

        /*
    printf("opcodes:\n");

    int cnt = 0;

    while(1) {
      if(*opcode_table[cnt].name == 0)
    break;

      printf("> %s -> [0x%04X]\n", opcode_table[cnt].name, opcode_table[cnt].code);
      cnt++;
    }
*/  

        //first pass
        s = text;
        int argN = 0;
        int argC = 4;
        unsigned int addr[2];
        addr[0] = 0;
        addr[1] = 0;


        unsigned int cSeg = 0;

        *text2 = 0;


        while(1) {
            char token[1000];
            char *t = token;

            while(strchr(" \t\n\r;", *s) && *s) {
                s++;
            }
            if(*s == '#') {
                while(*s && !strchr("\n\r", *s)) {
                    s++;
                }
            }
            if(*s == '"') {
                char strData[1024];
                char * sDat = strData;
                *sDat = '"';
                sDat++;
                s++;
                while(*s) {
                    if(*s == '"' && *(s-1) != '\\')
                            break;
                    *sDat = *s;
                    sDat++;
                    addr[cSeg]++;
                    s++;
                }
                s++;
                addr[cSeg]++;
                *sDat = '"';
                *(sDat + 1) = 0;
                strcpy(text2 + strlen(text2), strData);
            }

            while(!strchr(" \t\n\r;", *s) && *s) {
                *t = *s;
                t++; s++;
            }
            *t = 0;
            t = token;
            if(!*s)
                break;
            if(*t) {
                if(argN == 0) { //opcode
                    if(token[0] == '@') {
                        addLabel(token+1, addr[cSeg], cSeg);
                    } else if (token[0] == '.') {
                        int skip = 0;
                        if(!memcmp(token+1, "seg", 3)) { //change segment
                            cSeg = atoi(token + 4);
                            strcpy(text2 + strlen(text2), token);
                            strcpy(text2 + strlen(text2), "\n");

                        } else if(!(token[1])) { //default skip 1 word
                            skip = 1;
                        } else {
                            skip = atoi(token + 1);
                        }
                        addr[cSeg]+=skip; //zero-terminator

                        //add space for variable (epic hack)
                        if(skip) {
                            strcpy(text2 + strlen(text2), "\"");
                            for(int i = 0; i<skip-1; i++) {
                                strcpy(text2 + strlen(text2), "S");
                            }
                            strcpy(text2 + strlen(text2), "\"");
                        }
                    } else {
                        argN++;
                        strcpy(text2 + strlen(text2), token);
                        strcpy(text2 + strlen(text2), " ");
                    }


                } else if(argN < argC) {
                    argN++;
                    strcpy(text2 + strlen(text2), token);
                    strcpy(text2 + strlen(text2), " ");
                }
                if(argN >= argC) {
                    strcpy(text2 + strlen(text2), ";\n");
                    addr[cSeg] += 4;
                    argN = 0;
                }
            }
        }


        //second pass
        s = text2;
        argN = 0;
        argC = 0;

        unsigned int * tgt[2];
        tgt[0] = seg[0];
        tgt[1] = seg[1];
        cSeg = 0;

        if(outLinkName) {
            if(verbose)
                printf("Writing link information to %s\n", outLinkName);
            FILE* outLinkFd = fopen(outLinkName, "w");
            for(int i = 0; i<255;i++) {
                if(!(*label_array[i].name)) break;
                fprintf(outLinkFd, "%s 0x%04X\n", label_array[i].name, getLabelAbsAddress(label_array[i].name));
            }
        }

        if(verbose) {
            printf("Labels:\n");
            for(int i = 0; i<255; i++) {
                if(!(*label_array[i].name)) break;
                printf("[%s] -> [0x%04X] seg %d abs[0x%04X]\n", label_array[i].name, label_array[i].addr, label_array[i].segment, getLabelAbsAddress(label_array[i].name));
            }
            if(verbose == 2) printf("Text after first pass:\n%s\n", text2);
        }
        while(1) {
            char token[1000];
            unsigned int instruction[4];
            char *t = token;
            while(strchr(" \t\n\r;", *s) && *s) {
                s++;
            }
            if(*s == '#') {
                while(*s && !strchr("\n\r", *s)) {
                    s++;
                }
            }


            while(*s == '"' && *s) {
                s++;
                while(*s) {
                    if(*s == '"' && *(s-1)!='\\')
                        break;
                    if(*s != '\\' || *(s-1)=='\\') {
                        //printf("%04X ", *s);
                        *tgt[cSeg] = *s;
                        tgt[cSeg]++;
                    }

                    s++;
                }
                //printf("%04X\n", 0);
                *tgt[cSeg] = 0;
                tgt[cSeg]++;
                s++;
            }
            while(!strchr(" \t\n\r;", *s) && *s) {
                *t = *s;
                t++; s++;
            }
            *t = 0;
            t = token;
            if(!*s)
                break;
            if(*t) {
                //printf("token: %s\n", token);
                int flSkip = 0;
                if(argN == 0) { //opcode
                    if(!memcmp(t, ".seg", 4)) {
                        cSeg = atoi(t+4);
                        flSkip = 1;
                    } else {

                        int opcodeIdx = getOpcodeTableIndex(t);
                        if(opcodeIdx < 0) {
                            printf("Unknown opcode: \"%s\"! Stop.\n", t);
                            return -1;
                        } else {
                            //printf("0x%04X\n", opcode_table[opcodeIdx].code);
                            argC = opcode_table[opcodeIdx].argC;
                            instruction[0] = opcode_table[opcodeIdx].code;

                            argN++;
                        }
                    }
                } else if(argN < argC) {
                    int deref_cnt = 1;
                    while(t[0] == '*') {
                        t++;
                        deref_cnt++;
                    }
                    if(t[0]=='@') {
                        instruction[argN] = getLabelAbsAddress(t+1);
                        //printf("\n\n\t>>>>>replace %s with address 0x%04X\n\n", t+1, instruction[argN]);

                    } else {
                        instruction[argN] = strtol(t,0,0);
                    }
                    //printf("token %s dereference %d\n", t, deref_cnt);
                    instruction[0] |= (deref_cnt << (5 + 2*(argN-1)));
                    argN++;


                }
                if(argN >= argC && !flSkip) {
                    //printf("%04X %04X %04X %04X\n", instruction[0], instruction[1], instruction[2], instruction[3]);
                    for(int _i = 0; _i < 4; _i++) {
                        *tgt[cSeg] = instruction[_i];
                        tgt[cSeg]++;
                    }
                    argN = 0;
                }
            }
        }

    }

    memcpy(memImage + segmentOffset[0], seg[0], (0x10000 - segmentOffset[0])*sizeof(unsigned int));
    memcpy(memImage + segmentOffset[1], seg[1], (0x10000 - segmentOffset[1])*sizeof(unsigned int));

    unsigned int stop;
    for( stop = 0xffff; stop > 0; stop--) {
        if(memImage[stop] != 0)
            break;
    }
    if(verbose == 2) {
        for(unsigned int i = 0; i<stop; i+=4) {
            printf("%04x : %04X %04X %04X %04X\n", i, memImage[i+0], memImage[i+1], memImage[i+2], memImage[i+3]);

        }
        printf("STOP at 0x%04X\n", stop);

    }
    if(outName) {

        FILE * outfd = fopen(outName, "w");

        //fprintf(outfd, "v2.0 raw\n");

        for(unsigned int i = 0; i<stop; i+=4) {
            fprintf(outfd, "%04X %04X %04X %04X\n", memImage[i+0], memImage[i+1], memImage[i+2], memImage[i+3]);

        }
        fclose(outfd);
    }


    return 0;
}
