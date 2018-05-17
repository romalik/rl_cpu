#define LCD_CMD_ADDR 0x80
#define LCD_DATA_ADDR 0x81

#define LCD_CLEAR 0x02
#define LCD_SETADDR 0x01
#define LCD_SETCURSOR 0x03

#define PIPE_READ_CHUNK 4096

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <malloc.h>

#define COLS 53
#define ROWS 30
#define TEXT_START 4800


unsigned int fb[COLS*ROWS*2];
unsigned int col = 0;
unsigned int row = 0;



void outb(int port, int value) {
//push target
//push val
//iostore
	asm("iaddrf_b 0");
	asm("iaddrf_b 1");
	asm("iostore");
}

int inb(int port) {
//push target
//push val
//iostore
	asm("addrl_b -5");
	asm("iaddrf_b 0");
	asm("ioread");
	asm("store");
	asm("ret");
}





void scroll() {
  memcpy(fb, fb + COLS, (ROWS-1)*COLS);
  memset(fb+(ROWS-1)*COLS, 0, COLS);
}

void drawFB() {
  int cur = 0;
  int prev = COLS*ROWS;
  int end = COLS*ROWS;
  int needSetAddr = 1;  



  for(cur = 0; cur < end; cur++) {
    if(fb[cur] != fb[prev]) {
      fb[prev] = fb[cur];
      if(needSetAddr) {
        outb(LCD_CMD_ADDR, LCD_SETADDR);
        outb(LCD_DATA_ADDR, cur+TEXT_START);
        needSetAddr = 0;
      }
      outb(LCD_DATA_ADDR, fb[cur]);
    } else {
      needSetAddr = 1;
    }
    prev++;
  }

  outb(LCD_CMD_ADDR, LCD_SETCURSOR);
  outb(LCD_DATA_ADDR, col);
  outb(LCD_DATA_ADDR, row);
  
  
}

void resetFB() {
  memset(fb, 0, COLS*ROWS);
  memset(fb+COLS*ROWS, 0xffff, COLS*ROWS);
  drawFB();
}

void processChar(int ch) {
	
	//printf("[vt] recieve char %d [%c]\n", ch , ch);
	
  if(ch != '\n') {
    fb[row*COLS + col] = ch;
    col++;
  } else {
    col = 0;
    row++;
  }
  if(col>=COLS) {
    col = 0;
    row++;
  }
  if(row>=ROWS) {
    row = ROWS-1;
    scroll();
  }
}


int main() {
  int fd;  
  int * ch;
  int n;
  int * c;
  resetFB();
  drawFB();

  ch = (int *)malloc(PIPE_READ_CHUNK);

  mkfifo("/dev/pty", 0777);
  fd = open("/dev/pty", O_RDONLY);


  while(1) {
    n = read(fd, ch, PIPE_READ_CHUNK);
    c = ch;
    while(n) {
      processChar(*c);
      n--; 
      c++;
    }
    drawFB();
  } 
  return 0;
}
