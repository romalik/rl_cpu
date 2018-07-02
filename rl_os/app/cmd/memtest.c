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
