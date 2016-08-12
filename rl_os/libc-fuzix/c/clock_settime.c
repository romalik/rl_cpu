#include <time.h>
#include <unistd.h>
#include <errno.h>

int clock_settime(clockid_t clk_id, const struct timespec *tp)
{
  puts("clock_settime stub!");
  return 0;
#if 0
  switch(clk_id) {
  case CLOCK_REALTIME:
    stime(&tp->tv_sec);
    return 0;
  case CLOCK_MONOTONIC:
    return -EPERM;
  default:
    errno = EINVAL;
  }
  return -1;
#endif
}
