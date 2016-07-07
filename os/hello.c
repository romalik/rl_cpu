#include <stdio.h>

int main(int argc, char **argv) {
    int i = 0;
    while (i < 5) {
        int j;
        printf("Hello from hello.c!! %d\n", i);
        for (j = 0; j < 10000; j++) {
            j--;
            j++;
        }
        i++;
    }


    return 123;
}
