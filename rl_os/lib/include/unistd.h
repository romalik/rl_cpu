#ifndef __UNISTD_H
#define __UNISTD_H
#ifndef __TYPES_H
#include <types.h>
#endif
#include <syscall.h>
#include <sys/stat.h>
#ifndef SEEK_SET
#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2
#endif

#define STDIN_FILENO	0
#define STDOUT_FILENO	1
#define STDERR_FILENO	2

extern off_t lseek(int __fd, off_t __offset, int __whence);

extern int isatty(int __fd);

extern int lstat(const char *__pathname, struct stat *__buf);
extern int readlink(const char *__pathname, char *__buf, int __bufsiz);

extern unsigned int sleep(unsigned int __seconds);
extern int usleep(useconds_t __usecs);

extern char **environ;

extern const char * _findPath(const char *__pathname);
extern int execl(const char *__pathname, const char *__arg0, ...);
//extern int execle(const char *__pathname, const char *__arg0, ...);
extern int execlp(const char *__pathname, const char *__arg0, ...);
//extern int execlpe(const char *__pathname, const char *__arg0, ...);
extern int execv(const char *__pathname, char *const __argv[]);
extern int execve(const char *__pathname, char * const __argv[], char * const __envp[]);
extern int execvp(const char *__pathname, char *const __argv[]);
//extern int execvpe(const char *__pathname, char *const __argv[], char * const __envp[]);

extern char *ttyname(int __fd);
extern int ttyname_r(int __fd, char *__buf, size_t __size);
extern char *getlogin(void);
extern int getlogin_r(char * __buf, size_t __size);
extern int system(const char *);
extern int pause(void);
extern pid_t fork(void);
extern char *getcwd(char *, int);
extern void swab(const void * __from, void * __to, ssize_t __count);

extern long sysconf(int __name);
extern long fpathconf(int __fd, int __name);
extern long pathconf(const char *__path, int __name);
extern long _pathconf(int __name);



extern int open(const char *path, int flags, ...);
extern int close(int fd);
extern int creat(const char *path, mode_t mode);
extern int mknod(const char *path, mode_t mode, dev_t dev);
extern int link(const char *path, const char *path2);
extern int unlink(const char *path);
extern ssize_t read(int fd, void *buf, int len);
extern ssize_t write(int fd, const void *buf, int len);
extern int chdir(const char *path);
extern int sync(void);
extern int access(const char *path, int way);
extern int chmod(const char *path, mode_t mode);
extern int chown(const char *path, uid_t owner, gid_t group);
extern int dup(int fd);
extern pid_t getpid(void);
extern pid_t getppid(void);
extern uid_t getuid(void);
extern mode_t umask(mode_t);
extern int execve(const char *path, char * const argv[], char *const envp[]);
extern pid_t wait(int *status);
extern int setuid(uid_t uid);
extern int setgid(gid_t gid);
extern int ioctl(int fd, int request,...);
extern int brk(void *addr);
extern void *sbrk(size_t increment);
extern pid_t _fork(uint16_t flags, void *addr);
extern int mount(const char *dev, const char *path, int flags);
extern int umount(const char *dev);
extern sighandler_t signal(int signum, sighandler_t sighandler);
extern int dup2(int oldfd, int newfd);
extern int _pause(unsigned int dsecs);
extern int kill(pid_t pid, int sig);
extern int pipe(int *pipefds);
extern gid_t getgid(void);
extern uid_t geteuid(void);
extern gid_t getegid(void);
extern int chroot(const char *path);
extern int fcntl(int fd, int cmd, ...);
extern int fchdir(int fd);
extern int fchmod(int fd, mode_t mode);
extern int fchown(int fd, uid_t owner, gid_t group);
extern int mkdir(const char *path, mode_t mode);
extern int rmdir(const char *path);
extern pid_t setpgrp(void);
extern pid_t waitpid(pid_t pid, int *status, int options);
extern int uadmin(int cmd, int ctrl, void *ptr);
extern int nice(int prio);
extern int rename(const char *path, const char *newpath);
extern int flock(int fd, int op);
extern pid_t getpgrp(void);
extern int sched_yield(void);
extern int acct(const char *filename);
extern int setgroups(size_t size, const gid_t *groups);
extern int getgroups(int size, gid_t *groups);
extern int getrlimit(int resource, struct rlimit *rlim);
extern int setrlimit(int resource, const struct rlimit *rlim);
extern int setpgid(pid_t pid, pid_t pgrp);
extern pid_t setsid(void);
extern pid_t getsid(pid_t pid);
extern int socket(int af, int type, int pf);
extern int listen(int fd, int len);
extern int bind(int fd, const struct sockaddr *s, int len);
extern int connect(int fd, const struct sockaddr *s, int len);
extern int shutdown(int fd, int how);

/* asm syscall hooks with C wrappers */
extern int _getdirent(int fd, void *buf, int len);
extern int _stat(const char *path, struct _uzistat *s);
extern int _fstat(int fd, struct _uzistat *s);
extern int _getfsys(uint16_t dev, struct _uzifilesys *fs);
extern int _time(__ktime_t *t, uint16_t clock);
extern int _stime(const __ktime_t *t, uint16_t clock);
extern int _times(struct tms *t);
extern int _utime(const char *file, __ktime_t *buf);
extern int _uname(struct _uzisysinfoblk *uzib, int len);
extern int _profil(void *samples, uint16_t offset, uint16_t size, int16_t scale);
extern int _lseek(int fd, off_t *offset, int mode);
extern int _select(int nfd, uint16_t *base);
extern int _accept(int fd);
extern int _getsockaddrs(int fd, int type, struct sockaddr_in *addr);
extern int _sendto(int fd, const char *buf, size_t len, struct _sockio *uaddr);
extern int _recvfrom(int fd, char *buf, size_t len, struct _sockio *uaddr);

/* C library provided syscall emulation */
extern int stat(const char *path, struct stat *s);
extern int fstat(int fd, struct stat *s);
extern int alarm(uint16_t seconds);
extern time_t time(time_t *t);
extern int stime(const time_t *t);
extern int times(struct tms *tms);
extern int utime(const char *filename, const struct utimbuf *utim);
extern int uname(struct utsname *buf);
extern int profil(unsigned short *bufbase, size_t bufsize, unsigned long offset,
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

extern int gethostname(char *__name, size_t __len);
extern int sethostname(const char *__name, size_t __len);

#ifndef __STDLIB_H
extern void exit(int __status);
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
