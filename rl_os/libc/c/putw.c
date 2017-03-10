#include <stdio.h>

int putw(char n, FILE *f)
{
  if (fwrite(&n, sizeof(n), 1, f) != 1)
    return EOF;
  return 0;
}
