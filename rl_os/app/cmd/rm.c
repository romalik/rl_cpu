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
    int i /*, recurse = 0, interact =0 */ ;
    int err = 1;

    for (i = 1; i < argc; i++) {
	if (argv[i][0] != '-') {
	    if (unlink(argv[i])) {
	        writes(2, argv[0]);
	        writes(2, ": cannot remove '");
	        writes(2, argv[i]);
	        perror("'");
	    }
	}
    }
    return 0;
}
