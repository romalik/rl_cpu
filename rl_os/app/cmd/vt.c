#define LCD_CMD_ADDR 0x7fdc
#define LCD_DATA_ADDR 0x7fdd

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
        *(unsigned int *)(LCD_CMD_ADDR) = LCD_SETADDR;
        *(unsigned int *)(LCD_DATA_ADDR) = cur+TEXT_START;
        needSetAddr = 0;
      }
      *(unsigned int *)(LCD_DATA_ADDR) = fb[cur];
    } else {
      needSetAddr = 1;
    }
    prev++;
  }

  *(unsigned int *)(LCD_CMD_ADDR) = LCD_SETCURSOR;
  *(unsigned int *)(LCD_DATA_ADDR) = col;
  *(unsigned int *)(LCD_DATA_ADDR) = row;
  
  
}

void resetFB() {
  memset(fb, 0, COLS*ROWS);
  memset(fb+COLS*ROWS, 0xffff, COLS*ROWS);
  drawFB();
}

void processChar(int ch) {
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

  mkfifo("/pty", 0777);
  fd = open("/pty", O_RDONLY);

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
