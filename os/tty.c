#include <tty.h>

unsigned int tty_read(unsigned int minor) {
    int c;
    while ((c = UART) == 0) {
    }
    return c;
}

unsigned int tty_write(unsigned int minor, unsigned int val) {
    PORT_OUT = val;
    return 0;
}
