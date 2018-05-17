#ifndef __UNISTD_H
#define __UNISTD_H
#include <types.h>
#include <syscall.h>
#include <sys/stat.h>
#include <signal.h>
#include <sys/utsname.h>
#include <utime.h> 


#ifndef SEEK_SET
#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2
#endif

#define STDIN_FILENO	0
#define STDOUT_FILENO	1
#define STDERR_FILENO	2


struct rlimit {
    void * p;
};
struct sockaddr {
    void * p;
};
struct sockaddr_in {
    void * p;
};
struct _sockio {
    void * p;
};


extern char **environ;


off_t lseek(int __fd, off_t __offset, int __whence);

int isatty(int __fd);

int lstat(const char *__pathname, struct stat *__buf);
int readlink(const char *__pathname, char *__buf, int __bufsiz);

unsigned int sleep(unsigned int __seconds);
int usleep(useconds_t __usecs);


const char * _findPath(const char *__pathname);
int execl(const char *__pathname, const char *__arg0, ...);
//int execle(const char *__pathname, const char *__arg0, ...);
int execlp(const char *__pathname, const char *__arg0, ...);
//int execlpe(const char *__pathname, const char *__arg0, ...);
int execv(const char *__pathname, char *const __argv[]);
int execve(const char *__pathname, char * const __argv[], char * const __envp[]);
int execvp(const char *__pathname, char *const __argv[]);
//int execvpe(const char *__pathname, char *const __argv[], char * const __envp[]);

char *ttyname(int __fd);
int ttyname_r(int __fd, char *__buf, size_t __size);
char *getlogin(void);
int getlogin_r(char * __buf, size_t __size);
int system(const char *);
int pause(void);
pid_t fork(void);
pid_t vfork(void);
char *getcwd(char *, int);
void swab(const void * __from, void * __to, ssize_t __count);

long sysconf(int __name);
long fpathconf(int __fd, int __name);
long pathconf(const char *__path, int __name);
long _pathconf(int __name);



int open(const char *path, int flags, ...);
int close(int fd);
int creat(const char *path, mode_t mode);
int mknod(const char *path, mode_t mode, dev_t dev);
int link(const char *path, const char *path2);
int symlink(const char *path, const char *path2);
int unlink(const char *path);
ssize_t read(int fd, void *buf, int len);
ssize_t write(int fd, const void *buf, int len);
int chdir(const char *path);
int sync(void);
int access(const char *path, int way);
int chmod(const char *path, mode_t mode);
int chown(const char *path, uid_t owner, gid_t group);
int dup(int fd);
pid_t getpid(void);
pid_t getppid(void);
uid_t getuid(void);
mode_t umask(mode_t);
int execve(const char *path, char * const argv[], char *const envp[]);
pid_t wait(int *status);
int setuid(uid_t uid);
int setgid(gid_t gid);
int ioctl(int fd, int request,...);
int brk(void *addr);
void *sbrk(intptr_t increment);
int mount(const char *dev, const char *path, int flags);
int umount(const char *dev);
sighandler_t signal(int signum, sighandler_t sighandler);
int dup2(int oldfd, int newfd);
int _pause(unsigned int dsecs);
int kill(pid_t pid, int sig);
int pipe(int *pipefds);
gid_t getgid(void);
uid_t geteuid(void);
gid_t getegid(void);
int chroot(const char *path);
int fcntl(int fd, int cmd, ...);
int fchdir(int fd);
int fchmod(int fd, mode_t mode);
int fchown(int fd, uid_t owner, gid_t group);
int mkdir(const char *path, mode_t mode);
int mkfifo(const char *path, mode_t mode);
int rmdir(const char *path);
pid_t setpgrp(void);
pid_t waitpid(pid_t pid, int *status, int options);
int uadmin(int cmd, int ctrl, void *ptr);
int nice(int prio);
int rename(const char *path, const char *newpath);
int flock(int fd, int op);
pid_t getpgrp(void);
int sched_yield(void);
int acct(const char *filename);
int setgroups(size_t size, const gid_t *groups);
int getgroups(int size, gid_t *groups);
int getrlimit(int resource, struct rlimit *rlim);
int setrlimit(int resource, const struct rlimit *rlim);
int setpgid(pid_t pid, pid_t pgrp);
pid_t setsid(void);
pid_t getsid(pid_t pid);
int socket(int af, int type, int pf);
int listen(int fd, int len);
int bind(int fd, const struct sockaddr *s, int len);
int connect(int fd, const struct sockaddr *s, int len);
int shutdown(int fd, int how);
int reboot(int a, int b, int c);


/* asm syscall hooks with C wrappers */
int getdirent(int fd, void *buf, int len);
int getfsys(uint16_t dev, void *fs);
int select(int nfd, uint16_t *base);
int accept(int fd);
int getsockaddrs(int fd, int type, struct sockaddr_in *addr);
int sendto(int fd, const char *buf, size_t len, struct _sockio *uaddr);
int recvfrom(int fd, char *buf, size_t len, struct _sockio *uaddr);

/* C library provided syscall emulation */
int stat(const char *path, struct stat *s);
int fstat(int fd, struct stat *s);
int alarm(uint16_t seconds);
time_t time(time_t *t);
int stime(const time_t *t);
int utime(const char *filename, const struct utimbuf *utim);
int uname(struct utsname *buf);
int profil(unsigned short *bufbase, size_t bufsize, unsigned long offset,
                                    unsigned int scale);




#define _SC_ARG_MAX		1
#define _SC_CHILD_MAX		2
#define _SC_HOST_NAME_MAX	3
#define _SC_LOGIN_NAME_MAX	4
#define _SC_CLK_TCK		5
#define _SC_OPEN_MAX		6
#define _SC_PAGESIZE		7
#define _SC_RE_DUP_MAX		8
#define _SC_STREAM_MAX		9
#define _SC_SYMLOOP_MAX		10
#define _SC_TTY_NAME_MAX	11
#define _SC_TZNAME_MAX		12
#define _SC_VERSION		13
#define _SC_PHYS_PAGES		14
#define _SC_AVPHYS_PAGES	15
#define _SC_NPROCESSORS_CONF	16
#define _SC_NPROCESSORS_ONLN	17
/* TODO: SYS5 isms */
#define _SC_NGROUPS_MAX		18
#define _SC_JOB_CONTROL		19
#define _SC_SAVED_IDS		20

#define _SC_FUZIX_LOADAVG1	64
#define _SC_FUZIX_LOADAVG5	65
#define _SC_FUZIX_LOADAVG15	66

#define _PC_LINK_MAX		1
#define _PC_MAX_CANON		2
#define _PC_MAX_INPUT		3
#define _PC_NAME_MAX		4
#define _PC_PATH_MAX		5
#define _PC_PIPE_BUF		6
#define _PC_CHOWN_RESTRICTED	7
#define _PC_NO_TRUNC		8
#define _PC_VDISABLE		9

#define _POSIX_LINK_MAX		_pathconf(_PC_LINK_MAX)
#define _POSIX_MAX_CANON	_pathconf(_PC_MAX_CANON)
#define _POSIX_MAX_INPUT	_pathconf(_PC_MAX_INPUT)
#define _POSIX_NAME_MAX		_pathconf(_PC_NAME_MAX)
#define _POSIX_PATH_MAX		_pathconf(_PC_PATH_MAX)
#define _POSIX_PIPE_BUF		_pathconf(_PC_PIPE_BUF)
#define _POSIX_CHOWN_RESTRICTED _pathconf(_PC_CHOWN_RESTRICTED)
#define _POSIX_NO_TRUNC		_pathconf(_PC_NO_TRUNC)

/* POSIX: show that the clock_ API is present */
#define _POSIX_TIMERS
#define _POSIX_MONTONIC_CLOCK

int gethostname(char *__name, size_t __len);
int sethostname(const char *__name, size_t __len);

#ifndef __STDLIB_H
void exit(int __status);
#endif

#ifndef R_OK
#define R_OK	4	/* Test for read permission.  */
#define W_OK	2	/* Test for write permission.  */
#define X_OK	1	/* Test for execute permission.  */
#define F_OK	0	/* Test for existence.	*/
#endif

#define F_ULOCK	0
#define F_LOCK	1
#define F_TLOCK	2
#define F_TEST	3

#endif /* __UNISTD_H */
