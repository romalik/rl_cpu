#include <stdio.h>
#include <syscall.h>


int dup(int oldfd) {
  puts("dup stub!");
  return 0;
}

int dup2(int oldfd, int newfd) {
  puts("dup2 stub!");
  return 0;
}

