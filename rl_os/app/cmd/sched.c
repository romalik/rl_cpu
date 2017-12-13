
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>



int main(int argc, char **argv)
{
    unsigned int new_timeslice = 20;
    int fd = open("/dev/schedctl", O_WRONLY);
    if(argc > 1) {
      new_timeslice = atoi(argv[1]);
    }
    write(fd, &new_timeslice, 1);
    close(fd);

    return (0);
}
