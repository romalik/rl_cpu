#include <stdio.h>
#include <lthreads.h>

int global_var = 0;

void * worker(void * arg) {
  int i, j;
  for(i = 0; i<100; i++) {
    printf("%s: %d\n", arg, i);
    global_var++;
    for(j = 0; j<10000; j++) {
    }
  }

  lthread_exit();
  return 0;
}


int main() {
  lthread_t t1, t2;
  unsigned int * p1;
  int i, j;
  size_t maxSize = 0xffff;

  while(maxSize) {
    p1 = malloc(maxSize);
    if(p1) break;
    maxSize--;
  }

  if(maxSize) {
    free(p1);
    printf("maxSize : 0x%04x (%d words)\n", maxSize, maxSize);
  }

  lthread_create(&t1, &worker, &"w1");
  lthread_create(&t2, &worker, &"w2");

  maxSize = 0xffff;
  while(maxSize) {
    p1 = malloc(maxSize);
    if(p1) break;
    maxSize--;
  }

  if(maxSize) {
    free(p1);
    printf("maxSize : 0x%04x (%d words)\n", maxSize, maxSize);
  }
  for(i = 0; i<50; i++) {
    printf("main: %d\n", i);
    global_var++;
    for(j = 0; j<10000; j++) {
    }
  }



  lthread_join(&t1);
  lthread_join(&t2);

  printf("Joined!\n");
  printf("global_var: %d (should be 250)\n", global_var);

  maxSize = 0xffff;
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
