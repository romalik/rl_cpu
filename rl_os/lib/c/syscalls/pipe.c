#include <stdio.h>
#include <syscall.h>


int pipe(int pipefd[2]) {
  puts("pipe stub!");
  return 0;
}

