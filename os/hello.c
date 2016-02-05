#include <stdio.h>

int main(int argc, char **argv) {
    while(1) {
        int j;
        printf("Hello from hello.c!!\n");
        for(j = 0; j<10000; j++) {
            j--;
            j++;
        }
    }

    return 0;
}
