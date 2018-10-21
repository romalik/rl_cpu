#include <termios.h>
#include <unistd.h>

int tcdrain(int fd)
{
  struct termios t;
  if (ioctl(fd, TCGETS, (unsigned int *)&t))
    return -1;
  return ioctl(fd, TCSETSW, (unsigned int *)&t);
}
