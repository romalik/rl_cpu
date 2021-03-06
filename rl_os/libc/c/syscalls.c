#include <syscall.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <stdio.h>
#include <sys/types.h>
#include <stdio.h>
#include <fcntl.h>


int regrpc(size_t queue_user, size_t major, size_t type) {
  struct regrpcSyscall s;
  s.id = __NR_regrpc;
  s.queue_user = queue_user;
  s.major = major;
  s.type = type;
  syscall(&s);
  return 0;
}

int getdtablesize() {
return 10;
}

int reboot(int a, int b, int c) {
  puts("reboot stub!");
  return 1;

}
int access(const char * filename, int mode) {
  puts("access stub!");
  return 1;
}

int chdir(const char *path) {
    struct chdirSyscall s;
    s.id = __NR_chdir;
    s.path = path;

    syscall(&s);
    return s.res;
}
int close(int fd) {
    struct closeSyscall s;
    s.id = __NR_close;
    s.fd = fd;
    syscall(&s);
    return s.fd;
}

int dup(int oldfd) {
  struct dupSyscall s;
  s.id = __NR_dup;
  s.oldfd = oldfd;
  syscall(&s);
  return s.retval;
}

int dup2(int oldfd, int newfd) {
  struct dup2Syscall s;
  s.id = __NR_dup2;
  s.oldfd = oldfd;
  s.newfd = newfd;
  syscall(&s);
  return s.retval;
}

int execve(const char * filename, char * const argv[], char * const envp[]) {
    struct execSyscall s;
    s.id = __NR_execve;
    s.filename = filename;
    s.argv = (void *)argv;
    s.envp = (void *)envp;
    syscall(&s);
    return -1;
}

extern void __do_exit(int rv);

void _exit(int code) {
    struct exitSyscall s;
    s.id = __NR_exit;
    s.code = code;
    syscall(&s);
}

void exit(int status) {
    __do_exit(status);
    _exit(status);
}

int symlink(const char *target, const char *linkpath) {

  puts("symlink stub!");
  return 0;
}

int fchmod(int fd, mode_t mode) {
  puts("fchmod stub!");
  return 0;
}
pid_t fork(void) {
    struct forkSyscall s;
    s.id = __NR_fork;
    s.pid = 0;
    syscall(&s);
    return s.pid;
}
pid_t vfork(void) {
    struct forkSyscall s;
    s.id = __NR_fork;
    s.pid = 0;
    syscall(&s);
    return s.pid;
}
int fstat(int fd, struct stat *buf) {
  struct fstatSyscall s;
    s.id = __NR_fstat;
    s.fd = fd;
    s.buf = buf;
    syscall(&s);
    return 0;

}

pid_t getpid() {
  puts("getpid stub!");
  return 0;
}

uid_t getuid() {
  puts("getuid stub!");
  return 0;
}


int ioctl(int d, int request, unsigned int * data) {
    struct ioctlSyscall s;
    s.id = __NR_ioctl;
	s.fd = d;
    s.req = request;
    s.p = (void *)data;

    syscall(&s);
    return s.retval;
}

int kill(pid_t pid, int sig) {
    struct killSyscall s;
    s.id = __NR_kill;
    s.pid = pid;
    s.sig = sig;

    syscall(&s);
    return 0;
}
int mkdir(const char *path, mode_t mode) {
    struct mkdirSyscall s;
    s.id = __NR_mkdir;
    s.path = path;

    syscall(&s);
    return s.res;
}

int mount(const char *dev, const char *path, int flags) {
    struct mountSyscall s;
    s.id = __NR_mount;
    s.dev_path = dev;
    s.mount_point_path = path;
    s.flags = flags;
    syscall(&s);
    return s.res;
}

int umount(const char *path) {
    struct umountSyscall s;
    s.id = __NR_umount;
    s.mount_point_path = path;
    syscall(&s);
    return s.res;
}


int mkfifo(const char *path, mode_t mode) {
    struct mkfifoSyscall s;
    s.id = __NR_mkfifo;
    s.path = path;

    syscall(&s);
    return s.res;
}
int open(const char *filename, int flags, ...) {
    struct openSyscall s;
    s.id = __NR_open;
    s.filename = filename;
    s.mode = flags;
    syscall(&s);
    return s.mode;
}
int _pause(unsigned int t) {
  puts("_pause stub!");
  return 0;

}

int pause(void)
{

  puts("pause stub!");
  return 0;
  /*
  return _pause(0);
  */
}


int pipe(int pipefd[2]) {
  struct pipeSyscall s;
  s.id = __NR_pipe;
  syscall(&s);
  pipefd[0] = s.pipefd[0];
  pipefd[1] = s.pipefd[1];
  return 0;
}

int yield() {
	struct yieldSyscall s;
	s.id = __NR_yield;
	syscall(&s);
	return 0;
}

int read(int fd, void *buf, int count) {
    struct readSyscall s;
    s.id = __NR_read;
    s.fd = fd;
    s.size = count;
    s.buf = buf;
    syscall(&s);
    return s.size;
}

#define chunk 1024
int rename(const char *old, const char * new) {
  int fdOld;
  int fdNew;
  unsigned int buf[chunk];
  size_t n;
  unlink(new);
  fdOld = open(old, O_RDONLY);
  fdNew = open(new, O_WRONLY|O_CREAT);
  
  while((n = read(fdOld, buf, chunk)) > 0) {
    write(fdNew, buf, n);
  }

  close(fdOld);
  close(fdNew);
  unlink(old);
  puts("rename hack!");
  
  /*
  struct openSyscall s;
    s.id = __NR_open;
    s.filename = filename;
    s.mode = flags;
    syscall(&s);
    return s.mode;
    */
  return 0;
}
/*
sighandler_t signal(int signum, sighandler_t sighandler) {
  puts("signal stub!");
  return (sighandler_t)0;
}
*/
/* sleep.c
 */
/* Divide by ten in shifts. Would be nice if the compiler did that for us 8)

   FIXME: probably worth having a Z80 asm version of this */
static unsigned int div10quicki(unsigned int i)
{
	unsigned int q, r;
	q = (i >> 1) + (i >> 2);
	q = q + (q >> 4);
	q = q + (q >> 8);
	q >>= 3;
	r = i - (((q << 2) + q) << 1);
	return q + (r >> 9);
}

unsigned int sleep(unsigned int seconds)
{
  puts("sleep stub!");
  return 0;
  /*
	__ktime_t end, now;
	_time(&end, 1);	// in 1/10ths
	end.time += seconds * 10;
	if (_pause(seconds * 10) == 0)
		return 0;
	_time(&now, 1);
	return div10quicki(end.time - now.time);
  */
}

int stat(const char *name, struct stat *buf) {
    struct statSyscall s;
    s.id = __NR_stat;
    s.filename = name;
    s.buf = buf;
    syscall(&s);
    return 0;
}


int stime(const time_t *t)
{
  puts("stime stub!");
  return 0;
}

time_t time(time_t *t)
{
  puts("time stub!");
  return 0;
}

int times(struct tms * buf) {
  puts("times stub!");
  return 0;
}

/* usleep.c
 */

int usleep(useconds_t us)
{
  puts("usleep stub!");
  return 0;
//	return _pause(us/100000UL);
}

pid_t waitpid(pid_t pid, int *status, int options) {
    struct waitpidSyscall s;
    s.id = __NR_waitpid;
    s.pid = pid;
    s.status = status;
    s.options = options;
    syscall(&s);
    return s.pid;
}

pid_t wait(int * status) {
  return waitpid(-1,status,0);
}

int actual_write(int fd, const void *buf, int count) {
    struct writeSyscall s;
    s.id = __NR_write;
    s.fd = fd;
    s.size = count;
    s.buf = buf;
    syscall(&s);
    return s.size;
}

int write(int fd, const void * buf, int count) {
	size_t n = count;
	size_t write_now = 0;
	while(n) {
		write_now = actual_write(fd, buf, n);
		if(!write_now) break;
		n -= write_now;
		buf = (const void *) ((size_t)buf + write_now);
	}
	return count - n;
}


int unlink(const char *path){
  struct unlinkSyscall s;
  s.id = __NR_unlink;
  s.path = path;

  syscall(&s);
  return s.res;
}



int mknod(const char *path, mode_t mode, dev_t dev) { 
  struct mknodSyscall s;
  s.id = __NR_mknod; 
  s.mode = mode;
  s.path = path;
  s.major = (dev & 0xff00) >> 8;
  s.minor = (dev & 0x00ff);
  syscall(&s);
  return s.res; 
}


int sync(void){ 
    struct syncSyscall s;
    s.id = __NR_sync;
    syscall(&s);
    return 0; 

}


 int link(const char *path, const char *path2){ return 0; }
 int chmod(const char *path, mode_t mode){ return 0; }
 int chown(const char *path, uid_t owner, gid_t group){ return 0; }
 pid_t getppid(void){ return 0; }
 mode_t umask(mode_t mode){ return 0; }
 int setuid(uid_t uid){ return 0; }
 int setgid(gid_t gid){ return 0; }
 int brk(void *addr){ return 0; }
 void *sbrk(intptr_t increment){ return 0; }
 gid_t getgid(void){ return 0; }
 uid_t geteuid(void){ return 0; }
 gid_t getegid(void){ return 0; }
 int chroot(const char *path){ return 0; }
 int fcntl(int fd, int cmd, ...){ return 0; }
 int fchdir(int fd){ return 0; }
 int fchown(int fd, uid_t owner, gid_t group){ return 0; }
 int rmdir(const char *path){ return 0; }
 pid_t setpgrp(void){ return 0; }
 int nice(int prio){ return 0; }
 int flock(int fd, int op){ return 0; }
 pid_t getpgrp(void){ return 0; }
 int sched_yield(void){ return 0; }
 int acct(const char *filename){ return 0; }
 int setgroups(size_t size, const gid_t *groups){ return 0; }
 int getgroups(int size, gid_t *groups){ return 0; }
 int getrlimit(int resource, struct rlimit *rlim){ return 0; }
 int setrlimit(int resource, const struct rlimit *rlim){ return 0; }
 int setpgid(pid_t pid, pid_t pgrp){ return 0; }
 pid_t setsid(void){ return 0; }
 pid_t getsid(pid_t pid){ return 0; }
 int socket(int af, int type, int pf){ return 0; }
 int listen(int fd, int len){ return 0; }
 int bind(int fd, const struct sockaddr *s, int len){ return 0; }
 int connect(int fd, const struct sockaddr *s, int len){ return 0; }
 int shutdown(int fd, int how){ return 0; }

 int uadmin(int cmd, int ctrl, void *ptr) {return 0;}
 int getdirent(int fd, void *buf, int len){ return 0; }
 int getfsys(uint16_t dev, void *fs){ return 0; }
 off_t lseek(int fd, off_t offset, int mode){ return 0; }
 int select(int nfd, uint16_t *base){ return 0; }
 int accept(int fd){ return 0; }
 int getsockaddrs(int fd, int type, struct sockaddr_in *addr){ return 0; }
 int sendto(int fd, const char *buf, size_t len, struct _sockio *uaddr){ return 0; }
 int recvfrom(int fd, char *buf, size_t len, struct _sockio *uaddr){ return 0; }

 int alarm(uint16_t seconds){ return 0; }
 int utime(const char *filename, const struct utimbuf *utim){ return 0; }
 int uname(struct utsname *buf){ return 0; }
 int profil(unsigned short *bufbase, size_t bufsize, unsigned long offset,
                                    unsigned int scale){ return 0; }

