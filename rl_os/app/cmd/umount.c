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
    if(argc < 2) {
	writes(2, "usage: mount path\n");
	return 0;
    }

    umount(argv[1]);

    return 0;
}
