#include <stdio.h>
#include <unistd.h>

extern char * __ultostr(unsigned long val, int radix);
extern char * __ltostr(long val, int radix);


int main(int argc, char **argv) {
    int a = 0;
    int b = 0;
    long al = 0;
    long bl = 0;
    char buf[100];

    int mode;

    mode = _IONBF | __MODE_READ | __MODE_IOTRAN;

    printf("Mode : %d\n", mode);

    printf("This is a test for printf\n");
    printf("This is a test for printf\n");
    printf("This is a test for printf\n");
    printf("This is a test for printf\n");
    printf("This is a test for printf\n");
    
    printf("sprintf test\n");
    sprintf(buf, "%s %d %d\n", "string 12 23", 12, 23);
    printf("s: %s\n", buf);
    asm("nop");

    
    /*
    printf("This test will check different math routines\n");
    printf("test 123: %d ", 123);
    //puts(ltostr(123,10));
    printf("\n");
*/

    printf("fprintf test\n");
    fprintf(stdout, "%s %d %d\n", "string 12 23", 12, 23);

    printf("test 1 2 3: %d %d %d ", 1, 2, 3);
    printf("\n");

    printf("test 12345: %d ", 12345);
    //puts(ltostr(12345,10));
    printf("\n");


    return 0;
}
