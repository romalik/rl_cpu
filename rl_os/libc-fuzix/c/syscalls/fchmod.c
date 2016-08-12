#include <syscall.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
int fchmod(int fd, mode_t mode) {
  puts("fchmod stub!");
  return 0;
}
