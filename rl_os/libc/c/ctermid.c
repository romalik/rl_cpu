#include <stdio.h>
#include <string.h>

/* We have a real /dev/tty so this is a valid answer and
   to spec */

static char path[9] = "/dev/tty";

char *ctermid(char *p)
{
  if (p)
    return strcpy(p, path);
  else
    return path;
}
