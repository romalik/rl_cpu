#include <syscall.h>
#include <unistd.h>
#include <sys/types.h>

pid_t wait(int *status) {
  puts("wait stub!");
  return 0;
  /*
    struct waitpidSyscall s;
    printf("Waitpid called!\n");
    s.id = __NR_waitpid;
    s.pid = pid;
    syscall(&s);
    return s.pid;
    */
}
