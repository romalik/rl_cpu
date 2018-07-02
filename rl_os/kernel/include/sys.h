#ifndef SYS_H_
#define SYS_H_

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

extern unsigned int system_interrupt_stack[2048];

/* __NR_setup 0  */ int sys_none(void * scallStructPtr);
/* __NR_exit 1   */ int sys_exit(void * scallStructPtr);
/* __NR_fork 2   */ int sys_fork(void * scallStructPtr);
/* __NR_read 3   */ int sys_read(void * scallStructPtr);
/* __NR_write 4  */ int sys_write(void * scallStructPtr);
/* __NR_open 5   */ int sys_open(void * scallStructPtr);
/* __NR_close 6  */ int sys_close(void * scallStructPtr);
/* __NR_waitpid 7*/ int sys_waitpid(void * scallStructPtr);
///* __NR_creat 8  */ int sys_none,
///* __NR_link 9   */ int sys_none
/* __NR_unlink 10*/ int sys_unlink(void * scallStructPtr);
/* __NR_execve 11*/ int sys_execve(void * scallStructPtr);
/* __NR_chdir 12 */ int sys_chdir(void * scallStructPtr);
///* __NR_time 13  */ int sys_none,
///* __NR_mknod 14 */ int sys_none,
///* __NR_chmod 15 */ int sys_none,
///* __NR_chown 16 */ int sys_none,
///* __NR_break 17 */ int sys_none,
/* __NR_stat 18  */ int sys_stat(void * scallStructPtr);
///* __NR_lseek 19 */ int sys_none,
///* __NR_getpid 20*/ int sys_none,
///* __NR_mount 21 */ int sys_none,
///* __NR_umount 22*/ int sys_none,
///* __NR_setuid 23*/ int sys_none,
///* __NR_getuid 24*/ int sys_none,
///* __NR_stime 25 */ int sys_none,
///* __NR_ptrace 26*/ int sys_none,
///* __NR_alarm 27 */ int sys_none,
/* __NR_fstat 28 */ int sys_fstat(void * scallStructPtr);
///* __NR_pause 29 */ int sys_none,
///* __NR_utime 30 */ int sys_none,
///* __NR_stty 31  */ int sys_none,
///* __NR_gtty 32  */ int sys_none,
///* __NR_access 33*/ int sys_none,
///* __NR_nice 34  */ int sys_none,
///* __NR_ftime 35 */ int sys_none,
///* __NR_sync 36  */ int sys_none,
/* __NR_kill 37  */ int sys_kill(void * scallStructPtr);
///* __NR_rename 38*/ int sys_none,
/* __NR_mkdir 39 */ int sys_mkdir(void * scallStructPtr);
///* __NR_rmdir 40 */ int sys_none,
/* __NR_dup 41   */ int sys_dup(void * scallStructPtr);
/* __NR_pipe 42  */ int sys_pipe(void * scallStructPtr);
///* __NR_times 43 */ int sys_none,
///* __NR_prof 44  */ int sys_none,
///* __NR_brk 45   */ int sys_none,
///* __NR_setgid 46*/ int sys_none,
///* __NR_getgid 47*/ int sys_none,
///* __NR_signal 48*/ int sys_none,
///* __NR_geteuid 49*/ int sys_none,
///* __NR_getegid 50*/ int sys_none,
///* __NR_acct 51  */ int sys_none,
///* __NR_phys 52  */ int sys_none,
///* __NR_lock 53  */ int sys_none,
/* __NR_ioctl 54 */ int sys_ioctl(void * scallStructPtr);
///* __NR_fcntl 55 */ int sys_none,
///* __NR_mpx 56   */ int sys_none,
///* __NR_setpgid 57*/ int sys_none,
///* __NR_ulimit 58*/ int sys_none,
///* __NR_uname 59 */ int sys_none,
///* __NR_umask 60 */ int sys_none,
///* __NR_chroot 61*/ int sys_none,
///* __NR_ustat 62 */ int sys_none,
/* __NR_dup2 63  */ int sys_dup2(void * scallStructPtr);
///* __NR_getppid 64*/ int sys_none,
///* __NR_getpgrp 65*/ int sys_none,
///* __NR_setsid 66*/ int sys_none,
/* __NR_clone 67 */ int sys_clone(void * scallStructPtr);
/* __NR_mkfifo 68*/ int sys_mkfifo(void * scallStructPtr);
/* __NR_yield 69   */ int sys_yield(void * scallStructPtr);
/* __NR_regrpc 70 */ int sys_regrpc(void * scallStructPtr);




#endif
