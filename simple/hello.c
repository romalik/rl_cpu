#define PORT_OUT 0x7fff

void putc(char c) {
  *(char *)(PORT_OUT) = c;
}

void puts(char * s) {
  while(*s) {
    putc(*s);
    s++;
  }
}


void kernel_main() {
  puts("Hello from rl_cpu!\n");
  while(1) {}

}
