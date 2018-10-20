#include <string.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>

/* FIXME: need -r -v -i etc */
static void writes(int fd, const char *p)
{
    write(fd, p, strlen(p));
}


int main(int argc, const char *argv[])
{
    if(argc < 3) {
	writes(2, "usage: mount device path\n");
	return 0;
    }

    mount(argv[1], argv[2], 0);

    return 0;
}
