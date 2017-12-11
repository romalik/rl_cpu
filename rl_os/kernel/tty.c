#include <tty.h>
#include <kstdio.h>

unsigned int tty_read(unsigned int minor, unsigned int * buf, size_t n) {
  int to_read = 1;//n;
  int got = 0;
  while(to_read) {
    int c;
    while ((c = UART) == 0) {
    }
    *buf = c;
    buf++;
    to_read--;
    got++;
  }
    return got;
}

unsigned int tty_write(unsigned int minor, const unsigned int * buf, size_t n) {
  int to_write = n;
  while(to_write) {

    PORT_OUT = *buf;
    buf++;
    to_write--;
  }
    return n;
}
