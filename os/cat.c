#include <stdio.h>
#include <unistd.h>

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
    int i = 1;
    while (i < argc) {
        catFile(argv[i]);
        i++;
    }

    return 0;
}
