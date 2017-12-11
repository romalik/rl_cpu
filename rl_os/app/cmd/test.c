#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>



int main(int argc, char **argv) {

    char buf[100];
    int i;
    int n = 10;
    if(argc > 1) n = atoi(argv[1]);
    for(i = 0; i<n; i++) {
      int fd;
      sprintf(buf, "file-%d", i);
      fd = open(buf, O_TRUNC | O_CREAT | O_WRONLY);
      printf("writing %d bytes to %s\n", write(fd, buf, strlen(buf)+1), buf);
      close(fd);
    }

    return 0;
}
