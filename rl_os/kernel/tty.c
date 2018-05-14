#include <tty.h>
#include <kstdio.h>
#include <cb.h>

#define UART_BUFFER_SIZE 100

unsigned int uart_buffer_data[UART_BUFFER_SIZE];
struct circular_buffer uart_buffer;
unsigned int uart_interrupt_stack[50];

void uart_init() {
  cb_create_static(UART_BUFFER_SIZE, &uart_buffer, uart_buffer_data);
}

void uart_interrupt() {
  int c;
  while ((c = inb(UART)) != 0) {
    cb_push(&uart_buffer, c);
  }

}

unsigned int tty_open(unsigned int minor, FILE * fd) {
	return 0;
}

unsigned int tty_close(unsigned int minor, FILE * fd) {
	return 0;
}


unsigned int tty_read(unsigned int minor, unsigned int * buf, size_t n) {
  int to_read = n;
  int got = 0;
  while(to_read && uart_buffer.size) {
    int c;
//    while ((c = inb(UART)) == 0) {
//    }

    c = cb_pop(&uart_buffer);
    *buf = c;
    buf++;
    to_read--;
    got++;
  }
  if(got) {
    return got;
  } else {
    *buf = 0;
    //block here!
    return 1;
  }
}

unsigned int tty_write(unsigned int minor, const unsigned int * buf, size_t n) {
  int to_write = n;
  while(to_write) {

    outb(PORT_OUT, *buf);
    buf++;
    to_write--;
  }
    return n;
}
