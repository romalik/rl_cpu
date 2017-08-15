#include <kstdio.h>
#include <string.h>
#include "ata.h"
#include "rlfs3.h"
#include "types.h"
#include <sched.h>
#include <kernel_worker.h>
#include <memmap.h>
#include <syscall.h>





extern unsigned int ticks;

void system_interrupt(void *p, struct IntFrame *fr) {
    int scall_id;
    scall_id = *(unsigned int *)p;

    if (scall_id == __NR_write) {
        int sz_write = 0;
        struct writeSyscall *s = (struct writeSyscall *)p;
        sz_write = k_write(cProc->openFiles[s->fd], s->buf, s->size);
        //printf("KERNEL: write %d/%d words to %d\n", sz_write, s->size,  s->fd);
        s->size = sz_write;
        return;
    } else if (scall_id == __NR_read) {
        int sz_read = 0;
        struct readSyscall *s = (struct readSyscall *)p;
        sz_read = k_read(cProc->openFiles[s->fd], s->buf, s->size);
        //printf("KERNEL: read %d/%d words from %d\n", sz_read, s->size, s->fd);
        s->size = sz_read;
        return;
    } else if (scall_id == __NR_kill) {
        struct killSyscall *s = (struct killSyscall *)p;
        sendSig(s->pid, s->sig);
        return;
    } else if (scall_id == __NR_mkdir) {
        struct mkdirSyscall *s = (struct mkdirSyscall *)p;

        s->res = k_mkdir(s->path);

        return;

    } else if (scall_id == __NR_mkfifo) {
        struct mkfifoSyscall *s = (struct mkfifoSyscall *)p;

        s->res = k_mkfifo(s->path);

        return;
    } else if (scall_id == __NR_pipe) {
        struct pipeSyscall *s = (struct pipeSyscall *)p;
        int fd;
        char newPath[100];
        newPath[0] = 0;

        k_mkfifo(newPath);

        for (fd = 0; fd < MAX_FILES_PER_PROC; fd++) {
            if (!cProc->openFiles[fd]) {
                break;
            }
        }
        if (fd != MAX_FILES_PER_PROC) {
            cProc->openFiles[fd] = k_open(newPath, O_WRONLY);
            s->pipefd[1] = fd;
        }
        for (; fd < MAX_FILES_PER_PROC; fd++) {
            if (!cProc->openFiles[fd]) {
                break;
            }
        }
        if (fd != MAX_FILES_PER_PROC) {
            cProc->openFiles[fd] = k_open(newPath, O_RDONLY);
            s->pipefd[0] = fd;
        }
        return;
    } else if (scall_id == __NR_chdir) {
        struct chdirSyscall *s = (struct chdirSyscall *)p;
        struct stat st;
        k_stat(s->path, &st);
        if (S_ISDIR(st.st_mode)) {
            cProc->cwd.idx = st.st_ino;
            s->res = 0;
        } else {
            s->res = -1;
        }
        return;
    } else if (scall_id == __NR_unlink) {
        struct unlinkSyscall *s = (struct unlinkSyscall *)p;
        struct stat st;
        k_stat(s->path, &st);
        if (S_ISDIR(st.st_mode)) {
            s->res = -1;
        } else {
            s->res = k_unlink(s->path);
        }
        return;
    } else if (scall_id == __NR_stat) {
        struct statSyscall *s = (struct statSyscall *)p;
        k_stat(s->filename, s->buf);
        return;
    } else if (scall_id == __NR_fstat) {
        struct fstatSyscall *s = (struct fstatSyscall *)p;
        fs_stat(&(cProc->openFiles[s->fd]->node), s->buf);
        return;
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
            //printf("KERNEL: open %s as %d\n", s->filename, s->mode);
            return;
        }

        //printf("KERNEL: open failed %s\n", s->filename);
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
                //printf("KERNEL: close %d\n", s->fd);
                return;
            }
        }
                //printf("KERNEL: close failed %d\n", s->fd);
        s->fd = -1;
        return;
    } else if (scall_id == __NR_dup) {
        int fd;
        struct dupSyscall *s = (struct dupSyscall *)p;

        if(!cProc->openFiles[s->oldfd]) {
          s->retval = -1;
            //printf("dup(%d) -> %d\n", s->oldfd, s->retval);
          return;
        }

        for (fd = 0; fd < MAX_FILES_PER_PROC; fd++) {
            if (!cProc->openFiles[fd]) {
                break;
            }
        }
        if (fd != MAX_FILES_PER_PROC) {
            cProc->openFiles[fd] = cProc->openFiles[s->oldfd];
            cProc->openFiles[fd]->refcnt++;
            s->retval = fd;
            //printf("dup(%d) -> %d\n", s->oldfd, s->retval);
            return;
        }
        s->retval = -1;
        return;
    } else if (scall_id == __NR_dup2) {
        int fd;
        struct dup2Syscall *s = (struct dup2Syscall *)p;

        if(!cProc->openFiles[s->oldfd] || s->newfd<0 || s->newfd>MAX_FILES_PER_PROC ) {
          s->retval = -1;
            //printf("dup2(%d, %d) -> %d\n", s->oldfd, s->newfd, s->retval);
          return;
        }

        if(s->oldfd == s->newfd) {
          s->retval = s->newfd;
            //printf("dup2(%d, %d) -> %d\n", s->oldfd, s->newfd, s->retval);
          return;
        }

        if(cProc->openFiles[s->newfd]) {
          k_close(cProc->openFiles[s->newfd]);
        }

        cProc->openFiles[s->oldfd] = cProc->openFiles[s->oldfd];
        cProc->openFiles[s->oldfd]->refcnt++;
        s->retval = fd;
            //printf("dup2(%d, %d) -> %d\n", s->oldfd, s->newfd, s->retval);
        return;

    } else if (scall_id == __NR_fork) {
        di();
        addKernelTask(KERNEL_TASK_FORK, cProc->pid, 0);

        cProc->ap = fr->ap;
        cProc->bp = fr->bp;
        cProc->sp = fr->sp;
        cProc->pc = fr->pc;

        cProc->s = fr->s;
        cProc->d = fr->d;

        cProc->state = PROC_STATE_KWORKER;
        ei();
        resched_now();
        while (1) {
        } // wait for context switch
    } else if (scall_id == __NR_clone) {
        di();
        addKernelTask(KERNEL_TASK_CLONE, cProc->pid, 0);

        cProc->ap = fr->ap;
        cProc->bp = fr->bp;
        cProc->sp = fr->sp;
        cProc->pc = fr->pc;

        cProc->s = fr->s;
        cProc->d = fr->d;
        
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
        cProc->s = fr->s;
        cProc->d = fr->d;
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
        cProc->s = fr->s;
        cProc->d = fr->d;
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
        cProc->s = fr->s;
        cProc->d = fr->d;
        cProc->state = PROC_STATE_KWORKER;
        ei();
        resched_now();
        while (1) {
        } // wait for context switch
    } else {
        printf("Unknown syscall %d\n", scall_id);
    }
}
