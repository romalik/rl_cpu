#ifndef DIRENT_H__
#define DIRENT_H__

#include <unistd.h>

struct dirent {
    unsigned int d_name[31];
    unsigned int d_ino;
};

int opendir(const char *name);
int closedir(int fd);
struct dirent *readdir(int fd);

#endif
