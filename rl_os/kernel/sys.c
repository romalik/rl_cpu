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

unsigned int system_interrupt_stack[2048];

unsigned int decoder_stack[2048];

extern unsigned int ticks;

int sys_none(void * scallStructPtr) {
  int scall_id = ugetc(cProc, (size_t)scallStructPtr, 0, 14);
  printf("Unknown syscall %d from pid %d\n", scall_id, cProc->pid);
  return 0;
}

int (*sys_table[])(void * scallStructPtr) = {

    /* __NR_setup 0  */ sys_none,
    /* __NR_exit 1   */ sys_exit,
    /* __NR_fork 2   */ sys_fork,
    /* __NR_read 3   */ sys_read,
    /* __NR_write 4  */ sys_write,
    /* __NR_open 5   */ sys_open,
    /* __NR_close 6  */ sys_close,
    /* __NR_waitpid 7*/ sys_waitpid,
    /* __NR_creat 8  */ sys_none,
    /* __NR_link 9   */ sys_none,
    /* __NR_unlink 10*/ sys_unlink,
    /* __NR_execve 11*/ sys_execve,
    /* __NR_chdir 12 */ sys_chdir,
    /* __NR_time 13  */ sys_none,
    /* __NR_mknod 14 */ sys_none,
    /* __NR_chmod 15 */ sys_none,
    /* __NR_chown 16 */ sys_none,
    /* __NR_break 17 */ sys_none,
    /* __NR_stat 18  */ sys_stat,
    /* __NR_lseek 19 */ sys_none,
    /* __NR_getpid 20*/ sys_none,
    /* __NR_mount 21 */ sys_none,
    /* __NR_umount 22*/ sys_none,
    /* __NR_setuid 23*/ sys_none,
    /* __NR_getuid 24*/ sys_none,
    /* __NR_stime 25 */ sys_none,
    /* __NR_ptrace 26*/ sys_none,
    /* __NR_alarm 27 */ sys_none,
    /* __NR_fstat 28 */ sys_fstat,
    /* __NR_pause 29 */ sys_none,
    /* __NR_utime 30 */ sys_none,
    /* __NR_stty 31  */ sys_none,
    /* __NR_gtty 32  */ sys_none,
    /* __NR_access 33*/ sys_none,
    /* __NR_nice 34  */ sys_none,
    /* __NR_ftime 35 */ sys_none,
    /* __NR_sync 36  */ sys_none,
    /* __NR_kill 37  */ sys_kill,
    /* __NR_rename 38*/ sys_none,
    /* __NR_mkdir 39 */ sys_mkdir,
    /* __NR_rmdir 40 */ sys_none,
    /* __NR_dup 41   */ sys_dup,
    /* __NR_pipe 42  */ sys_pipe,
    /* __NR_times 43 */ sys_none,
    /* __NR_prof 44  */ sys_none,
    /* __NR_brk 45   */ sys_none,
    /* __NR_setgid 46*/ sys_none,
    /* __NR_getgid 47*/ sys_none,
    /* __NR_signal 48*/ sys_none,
    /* __NR_geteuid 49*/ sys_none,
    /* __NR_getegid 50*/ sys_none,
    /* __NR_acct 51  */ sys_none,
    /* __NR_phys 52  */ sys_none,
    /* __NR_lock 53  */ sys_none,
    /* __NR_ioctl 54 */ sys_ioctl,
    /* __NR_fcntl 55 */ sys_none,
    /* __NR_mpx 56   */ sys_none,
    /* __NR_setpgid 57*/ sys_none,
    /* __NR_ulimit 58*/ sys_none,
    /* __NR_uname 59 */ sys_none,
    /* __NR_umask 60 */ sys_none,
    /* __NR_chroot 61*/ sys_none,
    /* __NR_ustat 62 */ sys_none,
    /* __NR_dup2 63  */ sys_dup2,
    /* __NR_getppid 64*/ sys_none,
    /* __NR_getpgrp 65*/ sys_none,
    /* __NR_setsid 66*/ sys_none,
    /* __NR_clone 67 */ sys_clone,
    /* __NR_mkfifo 68*/ sys_mkfifo,
    /* __NR_yield 69*/ sys_yield,
    /* __NR_regrpc 70*/ sys_regrpc


    };

void in_sys_int() {
  printf("SYS IN\n");
}
void out_sys_int() {
  printf("SYS OUT\n");
}

void system_interrupt(/*, struct IntFrame *fr*/) {
  void * scallStructPtr;
  int scall_id;

  struct InterruptFrame * fr = (struct InterruptFrame *)system_interrupt_stack;
  scallStructPtr = (void *)ugetc(cProc, (size_t)fr->AP, 0, 14);
  scall_id = ugetc(cProc, (size_t)scallStructPtr, 0, 14);

  if(scall_id >= 0 && scall_id < __NR_N_SYSCALL) {
    sys_table[scall_id](scallStructPtr);
  } else {
    sys_none(scallStructPtr);
  }
}


void decoder_interrupt() {
  printf("Decoder fault!\nKilling\n");
  sendSig(cProc->pid, SIGKILL);
  resched(decoder_stack);

}

