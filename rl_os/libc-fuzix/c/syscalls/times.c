#include <stdio.h>
#include <syscall.h>
#include <sys/times.h>
#include <sys/types.h>

int times(struct tms * buf) {
  puts("times stub!");
  return 0;
}

