/* usleep.c
 */
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <syscall.h>
#include <stdio.h>

int usleep(useconds_t us)
{
  puts("usleep stub!");
  return 0;
//	return _pause(us/100000UL);
}
