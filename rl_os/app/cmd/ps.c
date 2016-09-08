#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>

#define PROCBUFSIZE 64*64
static char buf[PROCBUFSIZE];

void readProc(char *name) {
    int n = 0;
    int fd;
    fd = open(name, O_RDONLY);

    n = read(fd, buf, PROCBUFSIZE);
    write(1, buf, n);

    close(fd);
}

int main(int argc, char **argv) {

  printf("PID STATE CMD\n");
  readProc(argv[1]);
    return 0;
}
