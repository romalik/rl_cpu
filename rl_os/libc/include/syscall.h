#ifndef __SYSCALL_H
#define __SYSCALL_H

#include <sys/types.h>

void syscall(void *p);

#define __NR_setup 0 /* used only by init, to get system going */
#define __NR_exit 1
#define __NR_fork 2
#define __NR_read 3
#define __NR_write 4
#define __NR_open 5
#define __NR_close 6
#define __NR_waitpid 7
#define __NR_creat 8
#define __NR_link 9
#define __NR_unlink 10
#define __NR_execve 11
#define __NR_chdir 12
#define __NR_time 13
#define __NR_mknod 14
#define __NR_chmod 15
#define __NR_chown 16
#define __NR_break 17
#define __NR_stat 18
#define __NR_lseek 19
#define __NR_getpid 20
#define __NR_mount 21
#define __NR_umount 22
#define __NR_setuid 23
#define __NR_getuid 24
#define __NR_stime 25
#define __NR_ptrace 26
#define __NR_alarm 27
#define __NR_fstat 28
#define __NR_pause 29
#define __NR_utime 30
#define __NR_stty 31
#define __NR_gtty 32
#define __NR_access 33
#define __NR_nice 34
#define __NR_ftime 35
#define __NR_sync 36
#define __NR_kill 37
#define __NR_rename 38
#define __NR_mkdir 39
#define __NR_rmdir 40
#define __NR_dup 41
#define __NR_pipe 42
#define __NR_times 43
#define __NR_prof 44
#define __NR_brk 45
#define __NR_setgid 46
#define __NR_getgid 47
#define __NR_signal 48
#define __NR_geteuid 49
#define __NR_getegid 50
#define __NR_acct 51
#define __NR_phys 52
#define __NR_lock 53
#define __NR_ioctl 54
#define __NR_fcntl 55
#define __NR_mpx 56
#define __NR_setpgid 57
#define __NR_ulimit 58
#define __NR_uname 59
#define __NR_umask 60
#define __NR_chroot 61
#define __NR_ustat 62
#define __NR_dup2 63
#define __NR_getppid 64
#define __NR_getpgrp 65
#define __NR_setsid 66
#define __NR_clone 67
#define __NR_mkfifo 68
#define __NR_yield 69
#define __NR_regrpc 70

#define __NR_N_SYSCALL 71


struct syncSyscall {
	unsigned int id;
};

struct regrpcSyscall {
    unsigned int id;

        unsigned int queue_user;
        unsigned int major;
        unsigned int type;
};



struct creatSyscall {
    unsigned int id;
    const char *path;
    mode_t mode;
};

struct killSyscall {
    unsigned int id;
    int pid;
    int sig;
};

struct ioctlSyscall {
    unsigned int id;
	int fd;
    int req;
	int retval;
    void * p;
};

struct yieldSyscall {
    unsigned int id;
};


struct writeSyscall {
    unsigned int id;
    int fd;
    int size;
    const void *buf;
};

struct readSyscall {
    unsigned int id;
    int fd;
    int size;
    void *buf;
};

struct chdirSyscall {
    unsigned int id;
    const char *path;
    int res;
};

struct unlinkSyscall {
    unsigned int id;
    const char *path;
    int res;
};

struct rmdirSyscall {
    unsigned int id;
    const char *path;
    int res;
};


struct mkdirSyscall {
    unsigned int id;
    const char *path;
    int res;
};

struct mountSyscall {
    unsigned int id;
    const char *dev_path;
    const char *mount_point_path;
    unsigned int flags;
    int res;
};
struct umountSyscall {
    unsigned int id;
    const char *mount_point_path;
    int res;
};



struct mkfifoSyscall {
    unsigned int id;
    const char *path;
    int res;
};
struct mknodSyscall {
    unsigned int id;
    const char *path;
    int mode;
    int major;
    int minor;
    int res;
};
struct pipeSyscall {
    unsigned int id;
    int pipefd[2];
};




struct forkSyscall {
    unsigned int id;
    unsigned int pid;
};

struct execSyscall {
    unsigned int id;
    const char *filename;
    void *argv;
    void *envp;
};

struct openSyscall {
    unsigned int id;
    const char *filename;
    int mode;
};

struct closeSyscall {
    unsigned int id;
    int fd;
};

struct exitSyscall {
    unsigned int id;
    unsigned int code;
};

struct waitpidSyscall {
    unsigned int id;
    pid_t pid;
    int * status;
    int options;

};


struct statSyscall {
    unsigned int id;
    const char * filename;
    void * buf;
};

struct fstatSyscall {
    unsigned int id;
    int fd;
    void * buf;
};


struct dupSyscall {
  unsigned int id;
  int oldfd;
  int retval;
};

struct dup2Syscall {
  unsigned int id;
  int oldfd;
  int newfd;
  int retval;
};

struct cloneSyscall {
    unsigned int id;
    void * fn;
    void * stack;
    void * args;
    int retval;
};


#endif
