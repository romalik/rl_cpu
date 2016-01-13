#include <syscall.h>

extern void syscall(void *p);

void exit(int code) {
  struct ExitSyscall {
    unsigned int id;
    unsigned int code;
  } s;
  s.id = __NR_exit;
  s.code = code;
  syscall(&s);


}
