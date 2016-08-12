#include <sys/stat.h>
#include <stdio.h>

int fstat(int fd, struct stat *buf) {
  puts("fstat stub!");
  return 0;

/*
  struct statSyscall s;
    s.id = __NR_stat;
    s.filename = name;
    s.buf = buf;
    syscall(&s);
    return 0;
    */
}

