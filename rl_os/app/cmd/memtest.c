#include <malloc.h>
#include <stdio.h>


char * p1;
char * p2;
char * p3;
char * p4;

extern char __code_end;
extern char __data_end;

int main(int argc, char ** argv) {
    size_t maxSize = 0xffff;
    printf("__code_end 0x%04x, __data_end 0x%04x\n", &__code_end, &__data_end);


    printf("Testing malloc\n");
    p1 = malloc(0x10);
    printf("p1 is 0x%04x\n", p1);
    p2 = malloc(0x10);
    printf("p2 is 0x%04x\n", p2);
    p3 = malloc(0x20);
    printf("p3 is 0x%04x\n", p3);
    p4 = malloc(0x15);
    printf("p4 is 0x%04x\n", p4);
    free(p1);
    free(p2);
    free(p4);
    p4 = malloc(0x15);
    printf("p4 is 0x%04x\n", p4);
    free(p3);
    free(p4);
    
    while(maxSize) {
        p1 = malloc(maxSize);
        if(p1) break;
        maxSize--;
    }
    
    if(maxSize) {
        free(p1);
        printf("maxSize : 0x%04x (%d words)\n", maxSize, maxSize);
    }
    return 0;
}
