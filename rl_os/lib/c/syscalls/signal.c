#include <syscall.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>

sighandler_t signal(int signum, sighandler_t sighandler) {
  puts("signal stub!");
  return (sighandler_t)0;
}

