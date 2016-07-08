#include <dirent.h>

struct dirent tDirent;

int opendir(void *name) {
    return open(name, 'r');
}

struct dirent *readdir(int d) {
    int r = 0;
    r = read(d, &tDirent, 32);
    if (r != 32)
        return (struct dirent *)0;

    return &tDirent;
}

int closedir(int d) {
    return close(d);
}
