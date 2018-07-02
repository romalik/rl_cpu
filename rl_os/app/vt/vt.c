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


#include <tmt.h>



#define COLS 53
#define ROWS 30
#define TEXT_START 4800


unsigned int fb[COLS*ROWS*2];
unsigned int col = 0;
unsigned int row = 0;

int addCrToNl = 1;

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

void callback(tmt_msg_t m, struct TMT *vt, const void *a, void *p);

int main() {
  int fd;  
  int * ch;
  int n;
  int * c;
  
  struct TMT *vt = tmt_open(ROWS, COLS, callback, NULL, NULL);
    if (!vt) {
        perror("could not allocate terminal");
		return EXIT_FAILURE;
	}


  
  
  resetFB();
  drawFB();

  ch = (int *)malloc(PIPE_READ_CHUNK);

  mkfifo("/dev/pty", 0777);
  fd = open("/dev/pty", O_RDONLY);

	//tmt_write(vt, "Hello!\n\r1234\n\r\5678", 0);
	//tmt_write(vt, "2Hello!\n\r21234\n\r\25678", 0);
	//return 0;
	

  while(1) {
    n = read(fd, ch, PIPE_READ_CHUNK);
	c = ch;
	while(n) {
		tmt_write(vt, (const char *)c, 1);
		if(*c == '\n' && addCrToNl) {
			tmt_write(vt, "\r", 1);
		}
		n--;
		c++;
	}
	updateRedrawFB(vt);
  }
  tmt_close(vt);

/*
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
*/ 
  return 0;
}


void updateRedrawFB(struct TMT *vt) {
    /* grab a pointer to the virtual screen */
    const struct TMTSCREEN *s = tmt_screen(vt);
    const struct TMTPOINT *c = tmt_cursor(vt);
	size_t rr;
	size_t cc;
	for (rr = 0; rr < s->nline; rr++){
		if (s->lines[rr]->dirty){
			//printf("update row %d\n", row);
			for (cc = 0; cc < s->ncol; cc++){
				//printf("%c",c);
				fb[rr*COLS + cc] = s->lines[rr]->chars[cc].c;
			}
		}
	}
	tmt_clean(vt);
	col=c->c;
	row = c->r;
	drawFB();
	
}

void
callback(tmt_msg_t m, struct TMT *vt, const void *a, void *p)
{
    /* grab a pointer to the virtual screen */
    const struct TMTSCREEN *s = tmt_screen(vt);
    const struct TMTPOINT *c = tmt_cursor(vt);
	size_t row;
	size_t col;

    switch (m){
        case TMT_MSG_BELL:
            /* the terminal is requesting that we ring the bell/flash the
             * screen/do whatever ^G is supposed to do; a is NULL
             */
            printf("bing!\n");
            break;

        case TMT_MSG_UPDATE:
            break;

        case TMT_MSG_ANSWER:
            /* the terminal has a response to give to the program; a is a
             * pointer to a string */
 //           printf("terminal answered %s\n", (const char *)a);
            break;

        case TMT_MSG_MOVED:
            /* the cursor moved; a is a pointer to the cursor's TMTPOINT */
//            printf("cursor is now at %d,%d\n", c->r, c->c);
            break;
			
    }
}