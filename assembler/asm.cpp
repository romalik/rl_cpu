#include <stdio.h>
#include <string.h>
#include <stdlib.h>



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
*/


opcode_entry opcode_table[] = {
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
    {"", 0x00, 0}
};


typedef struct label_entry_ {
    char name[255];
    unsigned int addr;
} label_entry;

label_entry label_array[255];

void initLabelArray() {
    for(int i = 0; i<255; i++) {
        *label_array[i].name = 0;
        label_array[i].addr = 0;
    }
}

void addLabel(char * _name, unsigned int _addr) {
    for(int i = 0; i<255; i++) {
        if(!(*label_array[i].name)) {
            strcpy(label_array[i].name, _name);
            label_array[i].addr = _addr;
            return;
        }
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

int main(int argc, char ** argv) {
    if(argc > 1) {
        FILE * fd;
        fd = fopen(argv[1], "r");
        char * s = text;
        while(1) {
            int c;
            if((c = fgetc(fd)) == EOF)
                break;

            *s = (char)c;
            s++;

        }
        if(argc > 2)
            flSim = !strcmp(argv[2], "-sim");

        if(flSim) {
            printf("v2.0 raw\n");
        } else {
            printf("text:\n%s\n", text);
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
        unsigned int addr = 0;
        *text2 = 0;

        initLabelArray();

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
                while(*s && *s != '"') {
                    *sDat = *s;
                    sDat++;
                    addr++;
                    s++;
                }
                s++;
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
                        addLabel(token+1, addr);
                    } else if (token[0] == '.') {
                        int skip = 0;
                        if(!(token[1])) { //default skip 1 word
                            skip = 1;
                        } else {
                            skip = atoi(token + 1);
                        }
                        addr+=skip;
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
                    addr += 4;
                    argN = 0;
                }
            }
        }


        //second pass
        s = text2;
        argN = 0;
        argC = 0;


        if(!flSim) {
            printf("Labels:\n");
            for(int i = 0; i<255; i++) {
                if(!(*label_array[i].name)) break;
                printf("[%s] -> [0x%04X]\n", label_array[i].name, label_array[i].addr);
            }
            printf("Text after first pass:\n%s\n", text2);
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
                while(*s && *s != '"') {
                    printf("%04X ", *s);
                    s++;
                }
                printf("%04X\n", 0);
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
                if(argN == 0) { //opcode

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

                } else if(argN < argC) {
                    int deref_cnt = 1;
                    while(t[0] == '*') {
                        t++;
                        deref_cnt++;
                    }
                    if(t[0]=='@') {
                        instruction[argN] = getLabelAddress(t+1);
                        //printf("\n\n\t>>>>>replace %s with address 0x%04X\n\n", t+1, instruction[argN]);

                    } else {
                        instruction[argN] = strtol(t,0,0);
                    }
                    //printf("token %s dereference %d\n", t, deref_cnt);
                    instruction[0] |= (deref_cnt << (5 + 2*(argN-1)));
                    argN++;


                }

                if(argN >= argC) {
                    static int addr = 0;
                    if(flSim)
                        printf("%04X %04X %04X %04X\n", instruction[0], instruction[1], instruction[2], instruction[3]);
                    else
                        printf("%04x : %04X %04X %04X %04X\n", addr, instruction[0], instruction[1], instruction[2], instruction[3]);

                    addr+=4;
                    argN = 0;
                }
            }
        }

    }



    return 0;
}
