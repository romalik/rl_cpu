#include <lthreads.h>
#include <malloc.h>
#include <syscall.h>
#include <sys/wait.h>
 

int lthread_create(lthread_t * lthread, void *(fn)(void *), void * arg) {
    struct cloneSyscall s;

    printf("lthread_create for fn at 0x%04x\n", (unsigned int)fn);

    lthread->stack_ptr = malloc(DEFAULT_STACK_SIZE);
    s.id = __NR_clone;
    s.stack = lthread->stack_ptr;
    s.fn = fn;
    s.args = arg;
    syscall(&s);
    lthread->id = s.retval;
    return 0;
}

int lthread_join(lthread_t * lthread) {
    int status;
    waitpid(lthread->id, &status, 0);
    free(lthread->stack_ptr);
    return 0;
}

int lthread_exit() {
    _exit(0);
    return 0;
}
