#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>



int main(int argc, char **argv) {
    int fd;
    fd = open(argv[1], O_RDONLY);
    ioctl(fd, 0, (unsigned int *)argv[2]);
    close(fd);
    return 0;
}
