#include <stdio.h>

int main(int argc, char **argv) {
    int i = 0;
    int k = 0;
    while (i < 35) {
        int j;
        printf("Hello from hello.c!! %d\n", i);
        for (j = 0; j < 10000; j++) {
            for(k = 0; k < 20; k++) {
                j--;
                j++;
            }
        }
        i++;
    }

    return 123;
}
