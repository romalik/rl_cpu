#include <stdio.h>
#include <unistd.h>

extern char * __ultostr(unsigned long val, int radix);
extern char * __ltostr(long val, int radix);

unsigned long z = 0x11223344;

void phex(unsigned int v) {
  v = v & 0x0f;
  if(v < 10) {
    putchar('0'+v);
  } else {
    putchar('A'+v-10);
  }
}

void printLong(unsigned long a) {
  unsigned int al = *(unsigned int *)&a;
  unsigned int ah = *((unsigned int *)&a+1);
  phex(ah>>12);
  phex(ah>>8);
  phex(ah>>4);
  phex(ah);
  phex(al>>12);
  phex(al>>8);
  phex(al>>4);
  phex(al);
  putchar('\n');


}



unsigned long mul2(unsigned long a, unsigned long b) {
  unsigned long res = 0;
  while(b) {
    if(b&0x0001) {
      res += a;
    }
    a = a << 1;
    b = b >> 1;
  }
  return res;
}

int main(int argc, char **argv) {

  //  int a = 0;
  //  int b = 0;
    unsigned long al = 0;
    unsigned long bl = 0;
    unsigned long res = 0;
/*
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

    

    printf("fprintf test\n");
    fprintf(stdout, "%s %d %d\n", "string 12 23", 12, 23);

    printf("test 1 2 3: %d %d %d ", 1, 2, 3);
    printf("\n");

    printf("test 12345: %d ", 12345);
    //puts(ltostr(12345,10));
    printf("\n");

    printf("long multiplication test\n");
*/

    al = 0xfffe;
    while(al < 0x0001000c) {
      printLong(al);
      al++;
    }

    al = 0x12345678;
    bl = 0x90ABCDEF;

    //res = al*bl;



    printf("al ");
    printLong(al);
    res = al << 1;
    printLong(res);

    res = al >> 1;
    printLong(res);

    res = al * 0x1234;//mul2(al,0x1234);
    printLong(res);

//    printf("12345*34567 = 426729615 hex 0x%08X [should be 196F608F] high 0x%04X low 0x%04X\n", res, (res >> 16), (res & 0xffff));



    return 0;
}
