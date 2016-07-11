#include <stdio.h>
#include <unistd.h>
#include <dirent.h>

unsigned int buf[1];

void catFile(char *name) {
    int n = 0;
    int fd = open(name, 'r');

    while (n = read(fd, buf, 1)) {
        write(stdout, buf, 1);
    }

    close(fd);
}

int main(int argc, char **argv) {
    int fd;
    struct dirent *res;

    fd = opendir(".");
    res = readdir(fd);

    while (res) {
        printf("%s\n", res->d_name);
        res = readdir(fd);
    }

    closedir(fd);

    return 0;
}
