#include <kstdio.h>
#include <string.h>
#include "ata.h"
#include "rlfs3.h"
#include "types.h"
#include <sched.h>
#include <kernel_worker.h>
#include <memmap.h>
#include <syscall.h>
#include <mm.h>
#include <sys.h>
#include <sched.h>
extern unsigned int system_interrupt_stack[];

int sys_kill(void * scallStructPtr) {
    struct killSyscall s;
    ugets(cProc, (size_t)scallStructPtr, 0, 14, sizeof(struct killSyscall), 0, (unsigned int *)&s);
    sendSig(s.pid, s.sig);

    uputs(cProc, (size_t)scallStructPtr, 0, 14, sizeof(struct killSyscall), 0, (unsigned int *)&s);
    return 0;
}
int sys_chdir(void * scallStructPtr) {
    struct chdirSyscall s;
    struct stat st;
    unsigned int fn_buf[1024];

    ugets(cProc, (size_t)scallStructPtr, 0, 14, sizeof(struct chdirSyscall), 0, (unsigned int *)&s);
    ugets(cProc, (size_t)s.path, 0, 14, 1024, 1, fn_buf);


    k_stat(fn_buf, &st);
    if (S_ISDIR(st.st_mode)) {
      cProc->cwd.idx = st.st_ino;
      s.res = 0;
    } else {
      s.res = -1;
    }

    uputs(cProc, (size_t)scallStructPtr, 0, 14, sizeof(struct chdirSyscall), 0, (unsigned int *)&s);
    return 0;
}

int sys_fork(void * scallStructPtr) {
  printf("fork syscall called\n");
    addKernelTask(KERNEL_TASK_FORK, cProc->pid, scallStructPtr);
    cProc->state = PROC_STATE_KWORKER;
    printf("resched..\n");
    resched(system_interrupt_stack);
    printf("done,ret\n");
    return 0;
}

int sys_clone(void * scallStructPtr) {
  addKernelTask(KERNEL_TASK_CLONE, cProc->pid, scallStructPtr);
  cProc->state = PROC_STATE_KWORKER;
  resched(system_interrupt_stack);
  return 0;
}

int sys_execve(void * scallStructPtr) {
  printf("execve syscall called\n");
    addKernelTask(KERNEL_TASK_EXECVE, cProc->pid, scallStructPtr);
    cProc->state = PROC_STATE_KWORKER;
    resched(system_interrupt_stack);
    return 0;
}

int sys_waitpid(void * scallStructPtr) {
    addKernelTask(KERNEL_TASK_WAITPID, cProc->pid, scallStructPtr);
    cProc->state = PROC_STATE_KWORKER;
    resched(system_interrupt_stack);
    return 0;
}

int sys_exit(void * scallStructPtr) {
    addKernelTask(KERNEL_TASK_EXIT, cProc->pid, scallStructPtr);
    cProc->state = PROC_STATE_KWORKER;
    resched(system_interrupt_stack);
    return 0;
}
