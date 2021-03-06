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


int sys_sync(void * scallStructPtr) {
  block_sync();
  return 0;
}

int sys_write(void * scallStructPtr) {
  struct readSyscall s;
  ugets(cProc, (size_t)scallStructPtr, 0, 14, sizeof(struct writeSyscall), 0, (unsigned int *)&s);
  try_k_write(cProc->openFiles[s.fd], s.buf, s.size, cProc->pid, (size_t)scallStructPtr);
  return 0;
}

int sys_read(void * scallStructPtr) {
  unsigned int iobuf[1024];
  struct readSyscall s;
  ugets(cProc, (size_t)scallStructPtr, 0, 14, sizeof(struct readSyscall), 0, (unsigned int *)&s);
  try_k_read(cProc->openFiles[s.fd], s.buf, s.size, cProc->pid, (size_t)scallStructPtr);
  return 0;
}


int sys_mount(void * scallStructPtr) {
  unsigned int dev_path[512];
  unsigned int mount_point_path[512];
  struct mountSyscall s;

  ugets(cProc, (size_t)scallStructPtr, 0, 14, sizeof(struct mountSyscall), 0, (unsigned int *)&s);
  ugets(cProc, (size_t)s.dev_path, 0, 14, 512, 1, dev_path);
  ugets(cProc, (size_t)s.mount_point_path, 0, 14, 512, 1, mount_point_path);

  s.res = k_mount((const char *)dev_path, (const char *)mount_point_path);

  uputs(cProc, (size_t)scallStructPtr, 0, 14, sizeof(struct mountSyscall), 0, (unsigned int *)&s);
  return 0;

}

int sys_umount(void * scallStructPtr) {
  unsigned int mount_point_path[512];
  struct umountSyscall s;

  ugets(cProc, (size_t)scallStructPtr, 0, 14, sizeof(struct umountSyscall), 0, (unsigned int *)&s);
  ugets(cProc, (size_t)s.mount_point_path, 0, 14, 512, 1, mount_point_path);

  s.res = k_umount((const char *)mount_point_path);

  uputs(cProc, (size_t)scallStructPtr, 0, 14, sizeof(struct umountSyscall), 0, (unsigned int *)&s);
  return 0;

}



int sys_mkdir(void * scallStructPtr) {
  unsigned int iobuf[1024];
  struct mkdirSyscall s;

  ugets(cProc, (size_t)scallStructPtr, 0, 14, sizeof(struct mkdirSyscall), 0, (unsigned int *)&s);
  ugets(cProc, (size_t)s.path, 0, 14, 1024, 1, iobuf);

  s.res = k_mkdir(iobuf);

  uputs(cProc, (size_t)scallStructPtr, 0, 14, sizeof(struct mkdirSyscall), 0, (unsigned int *)&s);
  return 0;

}
int sys_mkfifo(void * scallStructPtr) {
  unsigned int iobuf[1024];
  struct mkfifoSyscall s;

  ugets(cProc, (size_t)scallStructPtr, 0, 14, sizeof(struct mkfifoSyscall), 0, (unsigned int *)&s);
  ugets(cProc, (size_t)s.path, 0, 14, 1024, 1, iobuf);

  s.res = k_mkfifo(iobuf);

  uputs(cProc, (size_t)scallStructPtr, 0, 14, sizeof(struct mkfifoSyscall), 0, (unsigned int *)&s);
  return 0;
}
int sys_mknod(void * scallStructPtr) {
  unsigned int iobuf[1024];
  struct mknodSyscall s;
  char type;

  if(s.mode & S_IFCHR) {
    type = 'c';
  } else if(s.mode & S_IFBLK) {
    type = 'b';
  }

  ugets(cProc, (size_t)scallStructPtr, 0, 14, sizeof(struct mknodSyscall), 0, (unsigned int *)&s);
  ugets(cProc, (size_t)s.path, 0, 14, 1024, 1, iobuf);

  s.res = k_mknod(iobuf,type,s.major,s.minor);

  uputs(cProc, (size_t)scallStructPtr, 0, 14, sizeof(struct mknodSyscall), 0, (unsigned int *)&s);
  return 0;
}
int sys_pipe(void * scallStructPtr) {
  struct pipeSyscall s;
  int fd;
  char newPath[100];
  newPath[0] = 0;

  ugets(cProc, (size_t)scallStructPtr, 0, 14, sizeof(struct pipeSyscall), 0, (unsigned int *)&s);


  k_mkfifo(newPath);

  for (fd = 0; fd < MAX_FILES_PER_PROC; fd++) {
    if (!cProc->openFiles[fd]) {
      break;
    }
  }
  if (fd != MAX_FILES_PER_PROC) {
    cProc->openFiles[fd] = k_open(newPath, O_WRONLY);
    s.pipefd[1] = fd;
  }
  for (; fd < MAX_FILES_PER_PROC; fd++) {
    if (!cProc->openFiles[fd]) {
      break;
    }
  }
  if (fd != MAX_FILES_PER_PROC) {
    cProc->openFiles[fd] = k_open(newPath, O_RDONLY);
    s.pipefd[0] = fd;
  }

  uputs(cProc, (size_t)scallStructPtr, 0, 14, sizeof(struct pipeSyscall), 0, (unsigned int *)&s);

  return 0;
}
int sys_unlink(void * scallStructPtr) {
  unsigned int iobuf[1024];
  struct unlinkSyscall s;
  struct stat st;

  ugets(cProc, (size_t)scallStructPtr, 0, 14, sizeof(struct unlinkSyscall), 0, (unsigned int *)&s);
  ugets(cProc, (size_t)s.path, 0, 14, 1024, 1, iobuf);

  k_stat(iobuf, &st);
  if (S_ISDIR(st.st_mode)) {
    s.res = -1;
  } else {
    s.res = k_unlink((const char *)iobuf);
  }

  uputs(cProc, (size_t)scallStructPtr, 0, 14, sizeof(struct unlinkSyscall), 0, (unsigned int *)&s);

  return 0;
}

int sys_stat(void * scallStructPtr) {
  unsigned int fn_buf[1024];
  struct stat st;
  struct statSyscall s;
  ugets(cProc, (size_t)scallStructPtr, 0, 14, sizeof(struct statSyscall), 0, (unsigned int *)&s);
  ugets(cProc, (size_t)s.filename, 0, 14, 1024, 1, fn_buf);

  k_stat(fn_buf, &st);

  uputs(cProc, (size_t)s.buf, 0, 14, sizeof(struct stat), 0, (unsigned int *)&st);
  uputs(cProc, (size_t)scallStructPtr, 0, 14, sizeof(struct statSyscall), 0, (unsigned int *)&s);

  return 0;
}

int sys_ioctl(void * scallStructPtr) {
  unsigned int fn_buf[1024];
  struct stat st;
  struct ioctlSyscall s;
  int retval;
  size_t sz;
  ugets(cProc, (size_t)scallStructPtr, 0, 14, sizeof(struct ioctlSyscall), 0, (unsigned int *)&s);
  ugets(cProc, (size_t)s.p, 0, 14, 1024, 0, fn_buf);

  retval = k_ioctl(cProc->openFiles[s.fd], s.req, fn_buf, &sz);

  s.retval = retval;
  if(sz) {
	uputs(cProc, (size_t)s.p, 0, 14, sz, 0, (unsigned int *)&st);
  }
  uputs(cProc, (size_t)scallStructPtr, 0, 14, sizeof(struct ioctlSyscall), 0, (unsigned int *)&s);

  return 0;
}

int sys_fstat(void * scallStructPtr) {
  struct fstatSyscall s;
  struct stat st;
  ugets(cProc, (size_t)scallStructPtr, 0, 14, sizeof(struct fstatSyscall), 0, (unsigned int *)&s);
  fs_stat(&(cProc->openFiles[s.fd]->node), &st);
  uputs(cProc, (size_t)s.buf, 0, 14, sizeof(struct stat), 0, (unsigned int *)&st);
  uputs(cProc, (size_t)scallStructPtr, 0, 14, sizeof(struct fstatSyscall), 0, (unsigned int *)&s);
  return 0;
}
int sys_open(void * scallStructPtr) {
  int fd;
  unsigned int fn_buf[1024];
  struct openSyscall s;
  ugets(cProc, (size_t)scallStructPtr, 0, 14, sizeof(struct openSyscall), 0, (unsigned int *)&s);
  ugets(cProc, (size_t)s.filename, 0, 14, 1024, 1, fn_buf);


  for (fd = 0; fd < MAX_FILES_PER_PROC; fd++) {
    if (!cProc->openFiles[fd]) {
      break;
    }
  }
  if (fd != MAX_FILES_PER_PROC) {
    cProc->openFiles[fd] = k_open(fn_buf, s.mode);
    s.mode = fd;
  } else {
    s.mode = -1;
  }
  uputs(cProc, (size_t)scallStructPtr, 0, 14, sizeof(struct openSyscall), 0, (unsigned int *)&s);

  return 0;
}
int sys_close(void * scallStructPtr) {
  int fd;
  struct closeSyscall s;
  ugets(cProc, (size_t)scallStructPtr, 0, 14, sizeof(struct closeSyscall), 0, (unsigned int *)&s);

  if ((s.fd >= 0) && (s.fd < MAX_FILES_PER_PROC)) {
    if (cProc->openFiles[s.fd]) {
      k_close(cProc->openFiles[s.fd]);
      cProc->openFiles[s.fd] = 0;
      s.fd = 0;
      goto sys_close_out;
    }
  }
  s.fd = -1;
  sys_close_out:
  uputs(cProc, (size_t)scallStructPtr, 0, 14, sizeof(struct closeSyscall), 0, (unsigned int *)&s);

  return 0;
}
int sys_dup(void * scallStructPtr) {
  int fd;
  struct dupSyscall s;

  ugets(cProc, (size_t)scallStructPtr, 0, 14, sizeof(struct dupSyscall), 0, (unsigned int *)&s);

  if(!cProc->openFiles[s.oldfd]) {
    s.retval = -1;
    goto sys_dup_out;
  }

  for (fd = 0; fd < MAX_FILES_PER_PROC; fd++) {
    if (!cProc->openFiles[fd]) {
      break;
    }
  }
  if (fd != MAX_FILES_PER_PROC) {
    cProc->openFiles[fd] = cProc->openFiles[s.oldfd];
    cProc->openFiles[fd]->refcnt++;
    s.retval = fd;
    goto sys_dup_out;
  }

  s.retval = -1;

  sys_dup_out:
  uputs(cProc, (size_t)scallStructPtr, 0, 14, sizeof(struct dupSyscall), 0, (unsigned int *)&s);

  return 0;
}
int sys_dup2(void * scallStructPtr) {
  int fd;
  struct dup2Syscall s;
  ugets(cProc, (size_t)scallStructPtr, 0, 14, sizeof(struct dup2Syscall), 0, (unsigned int *)&s);

  if(!cProc->openFiles[s.oldfd] || s.newfd<0 || s.newfd>MAX_FILES_PER_PROC ) {
    s.retval = -1;
    goto sys_dup2_out;
  }

  if(s.oldfd == s.newfd) {
    s.retval = s.newfd;
    goto sys_dup2_out;
  }

  if(cProc->openFiles[s.newfd]) {
    k_close(cProc->openFiles[s.newfd]);
  }

  cProc->openFiles[s.oldfd] = cProc->openFiles[s.oldfd];
  cProc->openFiles[s.oldfd]->refcnt++;
  s.retval = fd;

  sys_dup2_out:
  uputs(cProc, (size_t)scallStructPtr, 0, 14, sizeof(struct dup2Syscall), 0, (unsigned int *)&s);
  return 0;
}
