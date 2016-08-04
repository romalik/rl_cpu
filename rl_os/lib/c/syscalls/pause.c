#include <unistd.h>
#include <stdio.h>
int _pause(int t) {
  puts("_pause stub!");
  return 0;

}

int pause(void)
{

  puts("pause stub!");
  return 0;
  /*
  return _pause(0);
  */
}
