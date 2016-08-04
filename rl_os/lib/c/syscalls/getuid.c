#include <syscall.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>

uid_t getuid() {
  puts("getuid stub!");
  return 0;
}

