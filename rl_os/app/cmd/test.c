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


unsigned long divide(unsigned long dividend, unsigned long divisor) {

    unsigned long denom=divisor;
    unsigned long current = 1;
    unsigned long answer=0;

    if ( denom > dividend)
        return 0;

    if ( denom == dividend)
        return 1;

    while (denom <= dividend) {
        denom <<= 1;
        current <<= 1;
    }

    denom >>= 1;
    current >>= 1;

    while (current!=0) {
        if ( dividend >= denom) {
            dividend -= denom;
            answer |= current;
        }
        current >>= 1;
        denom >>= 1;
    }
    return answer;
}

unsigned long mod(unsigned long a, unsigned long b) {
  return a - (a/b)*b; //awful
}

int main(int argc, char **argv) {

    unsigned long al = 0;
    unsigned long bl = 0;
    unsigned long res = 0;
    int q,r;
    al = 143567;

    printf("gonna try to print 143567\nprintLong: ");
    printLong(al);
    printf("printf as %%lx : 0x%08lx\n", al);
    printf("printf as lu %lu\n", al);


    return 0;
}
