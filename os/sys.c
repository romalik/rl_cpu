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
        int sz_write = 0;
        struct writeSyscall *s = (struct writeSyscall *)p;
        sz_write = k_write(cProc->openFiles[s->fd], s->buf, s->size);
        s->size = sz_write;
        return;
    } else if (scall_id == __NR_read) {
        int sz_read = 0;
        struct readSyscall *s = (struct readSyscall *)p;
        sz_read = k_read(cProc->openFiles[s->fd], s->buf, s->size);
        s->size = sz_read;
        return;
    } else if (scall_id == __NR_time) {
        *((unsigned int *)(p) + 1) = ticks;
    } else if (scall_id == __NR_open) {
        int fd;
        struct openSyscall *s = (struct openSyscall *)p;

        for (fd = 0; fd < MAX_FILES_PER_PROC; fd++) {
            if (!cProc->openFiles[fd]) {
                break;
            }
        }
        if (fd != MAX_FILES_PER_PROC) {
            cProc->openFiles[fd] = k_open(s->filename, s->mode);
            s->mode = fd;
            return;
        }
        s->mode = -1;
        return;
    } else if (scall_id == __NR_close) {
        int fd;
        struct closeSyscall *s = (struct closeSyscall *)p;
        if ((s->fd >= 0) && (s->fd < MAX_FILES_PER_PROC)) {
            if (cProc->openFiles[s->fd]) {
                k_close(cProc->openFiles[s->fd]);
                cProc->openFiles[s->fd] = 0;
                s->fd = 0;
                return;
            }
        }
        s->fd = -1;
        return;

    } else if (scall_id == __NR_fork) {
        di();
        addKernelTask(KERNEL_TASK_FORK, cProc->pid, 0);

        cProc->ap = fr->ap;
        cProc->bp = fr->bp;
        cProc->sp = fr->sp;
        cProc->pc = fr->pc;

        cProc->state = PROC_STATE_KWORKER;
        ei();
        resched_now();
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
        resched_now();
        while (1) {
        } // wait for context switch
    } else if (scall_id == __NR_waitpid) {
        di();
        addKernelTask(KERNEL_TASK_WAITPID, cProc->pid, p);
        cProc->ap = fr->ap;
        cProc->bp = fr->bp;
        cProc->sp = fr->sp;
        cProc->pc = fr->pc;
        cProc->state = PROC_STATE_KWORKER;
        ei();
        resched_now();
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
        resched_now();
        while (1) {
        } // wait for context switch
    } else {
        printf("Unknown syscall %d\n", scall_id);
    }
}
