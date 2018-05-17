#define BUSYELKS1

#include <stdio.h>
#include "config.h"
#include "applets.h"
#include <string.h>


int main(int argc, char **argv)
{
	char *slash = strrchr(argv[0], '/');
	char progname[32];
	int q;

	if (!slash || (slash == argv[0] && !slash[1]))
		strcpy(progname, argv[0]);
	else strcpy(progname, (slash + 1));

	/* If our name was called, assume argv[1] is the command */
	if(strcmp(progname, "busyelks1") == 0) {
		strcpy(progname, argv[1]);
	}

#include "applets.c"

	if(strcmp(progname, "true") == 0) exit(0);
	if(strcmp(progname, "false") == 0) exit(1);
	if(strcmp(progname, "sync") == 0) { sync(); exit(0); }


usage();
}
