#include <tty.h>

unsigned int tty_read(unsigned int minor, unsigned int * buf, size_t n) {
  int to_read = n;
  while(to_read) {
    int c;
    while ((c = UART) == 0) {
    }
    *buf = c;
    buf++;
    to_read--;
  }
    return n;
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
