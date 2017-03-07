#include <stdio.h>
#include <unistd.h>

extern char * __ultostr(unsigned long val, int radix);
extern char * __ltostr(long val, int radix);

int main(int argc, char **argv) {
    int a = 0;
    int b = 0;
    long al = 0;
    long bl = 0;
    printf("This test will check different math routines\n");
    printf("test 123: %d ", 123);
    puts(__ltostr(123,10));
    printf("\n");


    asm("nop");

    printf("test 1 2 3: %d %d %d ", 1, 2, 3);
    printf("\n");

    printf("test 12345: %d ", 12345);
    puts(__ltostr(12345,10));
    printf("\n");

    printf("test 60000 d u tlostr ultostr: %d %u ", 60000, 60000);
    puts(__ltostr(60000,10));
    printf(" ");
    puts(__ultostr(60000,10));
    printf("\n");

    return 0;
}
