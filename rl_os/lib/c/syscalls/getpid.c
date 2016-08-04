#include <syscall.h>
#include <unistd.h>
#include <sys/types.h>

pid_t getpid() {
  puts("getpid stub!");
  return 0;
}
