#include <syscall.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>

int access(const char * filename, mode_t mode) {
  puts("access stub!");
  return 1;
}

