#include <syscall.h>
#include <unistd.h>
#include <sys/types.h>

int kill(pid_t pid, int sig) {
  puts("kill stub!");
  return 0;
}
