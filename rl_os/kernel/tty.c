#include <tty.h>
#include <kstdio.h>
#include <cb.h>

#define UART_BUFFER_SIZE 100

unsigned int uart_buffer_data[2][UART_BUFFER_SIZE];
struct circular_buffer uart_buffer[2];
unsigned int uart_interrupt_stack[50];

FILE * ttyOpenedAs[2];

void uart_init() {
  cb_create_static(UART_BUFFER_SIZE, &uart_buffer[0], uart_buffer_data[0]);
  cb_create_static(UART_BUFFER_SIZE, &uart_buffer[1], uart_buffer_data[1]);
}

void uart_interrupt() {
  int c;
  int got = 0;
  while ((c = inb(UART)) != 0) {
	  got = 1;
    cb_push(&uart_buffer[0], c);
  }
  
  if(got) waitqNotify((size_t)(ttyOpenedAs[0]->node.idx));

}

void uart2_interrupt() {
  int c;
  int got = 0;
  while ((c = inb(UART2)) != 0) {
	  got = 1;
    cb_push(&uart_buffer[1], c);
  }
  
  if(got) waitqNotify((size_t)(ttyOpenedAs[1]->node.idx));

}

unsigned int tty_open(unsigned int minor, FILE * fd) {
	ttyOpenedAs[minor] = fd;
	return 0;
}

unsigned int tty_close(unsigned int minor, FILE * fd) {
	return 0;
}


unsigned int tty_read(unsigned int minor, unsigned int * buf, size_t n) {
  int to_read = n;
  int got = 0;
  while(to_read && uart_buffer[minor].size) {
    int c;
//    while ((c = inb(UART)) == 0) {
//    }

    c = cb_pop(&uart_buffer[minor]);
    *buf = c;
    buf++;
    to_read--;
    got++;
  }
  if(got) {
    return got;
  } else {
	blockRequest = 1;
    return 0;
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

int tty_ioctl(unsigned int minor, unsigned int request, unsigned int * buf, size_t * sz, FILE * file) {
	printf("[tty_ioctl] minor = %d, request = %d\n", minor, request);
	*sz = 0;
	return 0;
}