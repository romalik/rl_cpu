#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char **argv) {

  unsigned long al = 0;
  unsigned long n;
  n = strtoul(argv[1],0,0);
  printf("n:%lu\n",n);
    for(al = 0; al < n; al++) {
      printf("%lu\n", al);
    }


    return 0;
}
