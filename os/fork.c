#include <syscall.h>
#include <fork.h>

unsigned int fork() {
  struct forkSyscall {
    unsigned int id;
    unsigned int pid;
  } s;
  s.id = __NR_fork;
  s.pid = 0;
  syscall(&s);
  return s.pid;

}
