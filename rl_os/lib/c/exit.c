/*
 *	Wrapper for clean up and then exit to the kernel
 *	via _exit
 */
#include <syscall.h>
#include <stdlib.h>
#include <unistd.h>

extern void __do_exit(int rv);

void _exit(int code) {
    struct exitSyscall s;
    s.id = __NR_exit;
    s.code = code;
    syscall(&s);
}

void exit(int status) {
    __do_exit(status);
    _exit(status);
}
