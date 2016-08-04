#include <stdio.h>
#include <sys/ioctl.h>

int ioctl(int d, int request, ...) {
  puts("ioctl stub!");
  return 0;
}

