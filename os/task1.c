#include <stdio.h>

int main(int argc, char ** argv) {
  int i;
  while(1) {
    printf("Hello from userland! Task1\n");
    for(i = 0; i<1000; i++) {
      i++;
      i--;
    }
  }
  return 0;
}
