#include <syscall.h>

extern void syscall(void *p);

void exit(int code) {
    struct exitSyscall s;
    s.id = __NR_exit;
    s.code = code;
    syscall(&s);
}
