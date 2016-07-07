#include <kstdio.h>
#include <string.h>
#include "ata.h"
#include "rlfs3.h"
#include "malloc.h"
#include "types.h"
#include <sched.h>
#include <kernel_worker.h>
#include <memmap.h>
#include <syscall.h>

/* Syscalls:
 *  1 - put char
 *    [1][c]
 *  2 - get char
 *    [2][&c]
 *  3 - get ticks
 *    [3][&t]
 */

extern unsigned int ticks;

void system_interrupt(void *p, struct IntFrame *fr) {
    int scall_id;
    scall_id = *(unsigned int *)p;
    //  printf("KERNEL: syscall %d %c\n", scall_id, (*((unsigned int *)(p)+1)));

    if (scall_id == __NR_write) {
        putc(*((unsigned int *)(p) + 1));
    } else if (scall_id == __NR_read) {
        *((unsigned int *)(p) + 1) = getc();
    } else if (scall_id == __NR_time) {
        *((unsigned int *)(p) + 1) = ticks;
    } else if (scall_id == __NR_fork) {
        di();
        addKernelTask(KERNEL_TASK_FORK, cProc->pid, 0);

        cProc->ap = fr->ap;
        cProc->bp = fr->bp;
        cProc->sp = fr->sp;
        cProc->pc = fr->pc;

        cProc->state = PROC_STATE_KWORKER;
        ei();
        while (1) {
        } // wait for context switch
    } else if (scall_id == __NR_execve) {
        di();
        addKernelTask(KERNEL_TASK_EXECVE, cProc->pid, p);
        cProc->ap = fr->ap;
        cProc->bp = fr->bp;
        cProc->sp = fr->sp;
        cProc->pc = fr->pc;
        cProc->state = PROC_STATE_KWORKER;
        ei();
        while (1) {
        } // wait for context switch
    } else if (scall_id == __NR_waitpid) {

        printf("sys.c : waitpid!\n");
        di();
        addKernelTask(KERNEL_TASK_WAITPID, cProc->pid, p);
        cProc->ap = fr->ap;
        cProc->bp = fr->bp;
        cProc->sp = fr->sp;
        cProc->pc = fr->pc;
        cProc->state = PROC_STATE_KWORKER;
        ei();
        while (1) {
        } // wait for context switch
    } else if (scall_id == __NR_exit) {
        di();
        addKernelTask(KERNEL_TASK_EXIT, cProc->pid, p);
        cProc->ap = fr->ap;
        cProc->bp = fr->bp;
        cProc->sp = fr->sp;
        cProc->pc = fr->pc;
        cProc->state = PROC_STATE_KWORKER;
        ei();
        while (1) {
        } // wait for context switch
    } else {
        printf("Unknown syscall %d\n", scall_id);
    }
}
