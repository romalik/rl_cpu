#include <stdio.h>
#include <open.h>
#include <close.h>

unsigned int buf[1];

void catFile(char *name) {
    int n = 0;
    int fd = open(name, 'r');
    printf("file %s opened as fd %d\n", name, fd);

    while (n = read(fd, buf, 1)) {
        write(stdout, buf, 1);
    }

    close(fd);
}

int main(int argc, char **argv) {
    printf("Cat!\n");
    //    while (i < argc) {
    //        catFile(argv[i]);
    //    catFile("/testf");
    //    }

    return 0;
}
