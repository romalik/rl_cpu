#include <syscall.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>

int rename(const char *old, const char * new) {
  puts("rename stub!");
  /*
  struct openSyscall s;
    s.id = __NR_open;
    s.filename = filename;
    s.mode = flags;
    syscall(&s);
    return s.mode;
    */
  return 0;
}
