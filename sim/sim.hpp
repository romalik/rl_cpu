#include <stdio.h>
#include <vector>
#include <list>
#include <map>
#include <deque>
#include <string.h>
#include <fstream>
#include <sstream>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include "oplist.h"
#include <SDL2/SDL.h>

#include <unistd.h>
#include <termios.h>
#include <mutex>
#include <thread>
#include <pthread.h>
#define SDL_SUPPORT 1

const int code_block_size = 8;
const int mPcMask = code_block_size-1;
typedef uint16_t w;
typedef int16_t ws;

static const unsigned char Font5x7[] = {
	0x00, 0x00, 0x00, 0x00, 0x00,// (space)
	0x00, 0x00, 0x5F, 0x00, 0x00,// !
	0x00, 0x07, 0x00, 0x07, 0x00,// "
	0x14, 0x7F, 0x14, 0x7F, 0x14,// #
	0x24, 0x2A, 0x7F, 0x2A, 0x12,// $
	0x23, 0x13, 0x08, 0x64, 0x62,// %
	0x36, 0x49, 0x55, 0x22, 0x50,// &
	0x00, 0x05, 0x03, 0x00, 0x00,// '
	0x00, 0x1C, 0x22, 0x41, 0x00,// (
	0x00, 0x41, 0x22, 0x1C, 0x00,// )
	0x08, 0x2A, 0x1C, 0x2A, 0x08,// *
	0x08, 0x08, 0x3E, 0x08, 0x08,// +
	0x00, 0x50, 0x30, 0x00, 0x00,// ,
	0x08, 0x08, 0x08, 0x08, 0x08,// -
	0x00, 0x60, 0x60, 0x00, 0x00,// .
	0x20, 0x10, 0x08, 0x04, 0x02,// /
	0x3E, 0x51, 0x49, 0x45, 0x3E,// 0
	0x00, 0x42, 0x7F, 0x40, 0x00,// 1
	0x42, 0x61, 0x51, 0x49, 0x46,// 2
	0x21, 0x41, 0x45, 0x4B, 0x31,// 3
	0x18, 0x14, 0x12, 0x7F, 0x10,// 4
	0x27, 0x45, 0x45, 0x45, 0x39,// 5
	0x3C, 0x4A, 0x49, 0x49, 0x30,// 6
	0x01, 0x71, 0x09, 0x05, 0x03,// 7
	0x36, 0x49, 0x49, 0x49, 0x36,// 8
	0x06, 0x49, 0x49, 0x29, 0x1E,// 9
	0x00, 0x36, 0x36, 0x00, 0x00,// :
	0x00, 0x56, 0x36, 0x00, 0x00,// ;
	0x00, 0x08, 0x14, 0x22, 0x41,// <
	0x14, 0x14, 0x14, 0x14, 0x14,// =
	0x41, 0x22, 0x14, 0x08, 0x00,// >
	0x02, 0x01, 0x51, 0x09, 0x06,// ?
	0x32, 0x49, 0x79, 0x41, 0x3E,// @
	0x7E, 0x11, 0x11, 0x11, 0x7E,// A
	0x7F, 0x49, 0x49, 0x49, 0x36,// B
	0x3E, 0x41, 0x41, 0x41, 0x22,// C
	0x7F, 0x41, 0x41, 0x22, 0x1C,// D
	0x7F, 0x49, 0x49, 0x49, 0x41,// E
	0x7F, 0x09, 0x09, 0x01, 0x01,// F
	0x3E, 0x41, 0x41, 0x51, 0x32,// G
	0x7F, 0x08, 0x08, 0x08, 0x7F,// H
	0x00, 0x41, 0x7F, 0x41, 0x00,// I
	0x20, 0x40, 0x41, 0x3F, 0x01,// J
	0x7F, 0x08, 0x14, 0x22, 0x41,// K
	0x7F, 0x40, 0x40, 0x40, 0x40,// L
	0x7F, 0x02, 0x04, 0x02, 0x7F,// M
	0x7F, 0x04, 0x08, 0x10, 0x7F,// N
	0x3E, 0x41, 0x41, 0x41, 0x3E,// O
	0x7F, 0x09, 0x09, 0x09, 0x06,// P
	0x3E, 0x41, 0x51, 0x21, 0x5E,// Q
	0x7F, 0x09, 0x19, 0x29, 0x46,// R
	0x46, 0x49, 0x49, 0x49, 0x31,// S
	0x01, 0x01, 0x7F, 0x01, 0x01,// T
	0x3F, 0x40, 0x40, 0x40, 0x3F,// U
	0x1F, 0x20, 0x40, 0x20, 0x1F,// V
	0x7F, 0x20, 0x18, 0x20, 0x7F,// W
	0x63, 0x14, 0x08, 0x14, 0x63,// X
	0x03, 0x04, 0x78, 0x04, 0x03,// Y
	0x61, 0x51, 0x49, 0x45, 0x43,// Z
	0x00, 0x00, 0x7F, 0x41, 0x41,// [
	0x02, 0x04, 0x08, 0x10, 0x20,// "\"
	0x41, 0x41, 0x7F, 0x00, 0x00,// ]
	0x04, 0x02, 0x01, 0x02, 0x04,// ^
	0x40, 0x40, 0x40, 0x40, 0x40,// _
	0x00, 0x01, 0x02, 0x04, 0x00,// `
	0x20, 0x54, 0x54, 0x54, 0x78,// a
	0x7F, 0x48, 0x44, 0x44, 0x38,// b
	0x38, 0x44, 0x44, 0x44, 0x20,// c
	0x38, 0x44, 0x44, 0x48, 0x7F,// d
	0x38, 0x54, 0x54, 0x54, 0x18,// e
	0x08, 0x7E, 0x09, 0x01, 0x02,// f
	0x08, 0x14, 0x54, 0x54, 0x3C,// g
	0x7F, 0x08, 0x04, 0x04, 0x78,// h
	0x00, 0x44, 0x7D, 0x40, 0x00,// i
	0x20, 0x40, 0x44, 0x3D, 0x00,// j
	0x00, 0x7F, 0x10, 0x28, 0x44,// k
	0x00, 0x41, 0x7F, 0x40, 0x00,// l
	0x7C, 0x04, 0x18, 0x04, 0x78,// m
	0x7C, 0x08, 0x04, 0x04, 0x78,// n
	0x38, 0x44, 0x44, 0x44, 0x38,// o
	0x7C, 0x14, 0x14, 0x14, 0x08,// p
	0x08, 0x14, 0x14, 0x18, 0x7C,// q
	0x7C, 0x08, 0x04, 0x04, 0x08,// r
	0x48, 0x54, 0x54, 0x54, 0x20,// s
	0x04, 0x3F, 0x44, 0x40, 0x20,// t
	0x3C, 0x40, 0x40, 0x20, 0x7C,// u
	0x1C, 0x20, 0x40, 0x20, 0x1C,// v
	0x3C, 0x40, 0x30, 0x40, 0x3C,// w
	0x44, 0x28, 0x10, 0x28, 0x44,// x
	0x0C, 0x50, 0x50, 0x50, 0x3C,// y
	0x44, 0x64, 0x54, 0x4C, 0x44,// z
	0x00, 0x08, 0x36, 0x41, 0x00,// {
	0x00, 0x00, 0x7F, 0x00, 0x00,// |
	0x00, 0x41, 0x36, 0x08, 0x00,// }
	0x08, 0x08, 0x2A, 0x1C, 0x08,// ->
	0x08, 0x1C, 0x2A, 0x08, 0x08 // <-
};

class VMemDevice {
public:
  VMemDevice() {};
  virtual ~VMemDevice() {};

  virtual int canOperate(w addr) = 0;
  virtual void write(w addr, w val, int seg, int force = 0) = 0;
  virtual w read(w addr, int seg) = 0;
  virtual void terminate() {};
};

class InterruptController : public VMemDevice {
  int currentInterrupt;
  w pendingInterrupts;
  w addrStart;
  int nIRQs;
  std::vector<w> intVectors;
 public:
  InterruptController(w _addrStart, int _nIRQs) {
     addrStart = _addrStart;
     nIRQs = _nIRQs;
     intVectors.resize(nIRQs, 0);
     currentInterrupt = 0;
     pendingInterrupts = 0;
  }
  ~InterruptController() {}

  virtual int canOperate(w addr) {
    return ((addr >= addrStart)&&(addr < addrStart + nIRQs));
  }

  virtual void write(w addr, w val, int seg, int force = 0) {
    if(canOperate(addr)) {
      intVectors[addr - addrStart] = val;
    }
  }

  virtual w read(w addr, int seg) {
    if(canOperate(addr)) {
      return intVectors[addr - addrStart];
    } else {
      return 0;
    }
  }

  void request(int irq) {
    //printf("IRQ on line %d\n", irq);

    if(irq < nIRQs) {
      pendingInterrupts |= (1 << irq);
    }

    //printf("Mask now 0x%04x\n", pendingInterrupts);
  }

  int irqLineStatus() {
    if(pendingInterrupts) {
      return 1;
    } else {
      return 0;
    }
  }

  w getIrqVector() {
    //printf("InterruptController: finding interrupt. Mask: 0x%04x\n", pendingInterrupts);
    if(!pendingInterrupts) {
      printf("Interrupt controller failure\n");
      return 0x1234;
    }

    while(true) {
      if(pendingInterrupts & (1 << currentInterrupt)) {
        break;
      }
      currentInterrupt++;
      if(currentInterrupt == nIRQs) {
        currentInterrupt = 0;
      }
    }


    pendingInterrupts &= ~(1<<currentInterrupt);

    //printf("InterruptController: done. Mask: 0x%04x\n", pendingInterrupts);
    return intVectors[currentInterrupt];
  }

};

class HDD : public VMemDevice {
  unsigned char * data;
  std::string image_path;
  int size;
  w cmdAddr;
  w dataAddr;
  int cSector;
  int cState;
  int cPos;
  static const int STATE_READING = 1;
  static const int STATE_WRITING = 2;


  static const int CMD_READ = 1;
  static const int CMD_WRITE = 2;
  static const int CMD_RESET = 3;
 public:
  HDD(w _cmdAddr, w _dataAddr, std::string path) {
    image_path = path;
    std::ifstream is(path.c_str(), std::ifstream::binary);
    is.seekg (0, is.end);
    size = is.tellg();
    is.seekg (0, is.beg);
    data = (unsigned char *)(malloc(size));
    is.read((char *)data, size);
    is.close();
    cSector = 0;
    cState = 0;
    cPos = 0;
    cmdAddr = _cmdAddr;
    dataAddr = _dataAddr;
    printf("HDD: create cmd:0x%04x data:0x%04x\n", cmdAddr, dataAddr);
  }
  virtual void terminate() {
    std::ofstream os(image_path.c_str(), std::ofstream::binary);
    os.write((char *)data, size);
    os.close();
  }
  virtual int canOperate(w addr) {
    return (addr == cmdAddr) || (addr == dataAddr);
  }
  virtual void write(w addr, w val, int seg, int force) {
    if(canOperate(addr)) {
//      printf("HDD: port 0x%04x val 0x%04x\n", addr, val);
      if(addr == cmdAddr) {
        if(val == CMD_READ) {
          cState = -STATE_READING;
          cPos = 0;
        } else if(val == CMD_WRITE) {
          cState = -STATE_WRITING;
          cPos = 0;
        } else if(val == CMD_RESET) {
//          printf("HDD: reset\n");
          cState = 0;
          cSector = 0;
          cPos = 0;
        }
      } else if(addr == dataAddr) {
        if(cState < 0) {
          if(cPos == 0) {
            cSector = ((int)val) << 16;
            cPos++;
          } else if(cPos == 1) {
            cSector |= (int)(val);
            cPos = 0;
//            printf("HDD: set sector %d\n", cSector);
            cState = -cState;
          }
        } else {
          if(cState == STATE_WRITING) {
            if(cPos < 512) {
              int dest = cSector*512 + cPos;
              data[dest] = val&0xff;
              data[dest+1] = ((val&0xff00)>>8);
      //        printf("HDD write 0x%04x at %d\n", val, dest);
      //        printf("dest : 0x%02x dest+1 : 0x%02x\n",data[dest], data[dest+1]);
              cPos+=2;
            } else {
              cState = 0;
            }
          }

        }
      }
    }
  }
  w read(w addr, int seg) {
    if(canOperate(addr)) {
      if(addr == cmdAddr) {
        return 0;
      } else if(addr == dataAddr) {
        if(cState < 0) {
          return 0;
        } else {
          if(cState == STATE_READING) {
            if(cPos < 512) {
              int dest = cSector*512 + cPos;
	      if(dest + 1 >= size) {
                printf("HDD: request out of bounds! cSector %d cPos %d\n", cSector, cPos); 
                return 0;  
	      }
              w retval = data[dest];
              retval |= ((data[dest+1])<<8);
              cPos+=2;
              if(cPos == 512) {
                cState = 0;
              }
              return retval;
            }
          }

        }
      }
    }
    return 0;
  }

};

class LCD : public VMemDevice {
  
  std::vector<w> data;
  w size;
  w cmdAddr;
  w dataAddr;
  w cIdx;

  static const w CMD_SETADDR = 0x01;
  static const w CMD_CLEAR = 0x02;
  static const w CMD_SETCURSOR = 0x03;

  static const uint32_t BG_COLOR = 0x0055ff55;
  static const uint32_t FG_COLOR = 0x00000000;

  int state{0};

  int width{0};
  int height{0};

  int cursor_x{0};
  int cursor_y{0};
  int cursor_visible{1};
  int cursor_blink_state{1};


  std::mutex vbufLock;
#if SDL_SUPPORT
  SDL_Window* window{NULL};
  SDL_Surface* screenSurface{NULL};
  SDL_Renderer* renderer{NULL};


  void initSDL() {
    if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
    {
        printf( "SDL could not initialize! SDL_Error: %s\n", SDL_GetError() );
    }
    else
    {
        //Create window
        window = SDL_CreateWindow( "LCD", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width*2, height*2, SDL_WINDOW_SHOWN );
//        SDL_CreateWindowAndRenderer(vp->w, vp->h, SDL_WINDOW_SHOWN, &window, &renderer);
        if( window == NULL )
        {
            printf( "Window could not be created! SDL_Error: %s\n", SDL_GetError() );
        }
        else
        {
            //Get window surface
            screenSurface = SDL_GetWindowSurface( window );

            //Fill the surface white
            SDL_FillRect( screenSurface, NULL, SDL_MapRGB( screenSurface->format, 0xFF, 0xFF, 0xFF ) );

            //Update the surface
            SDL_UpdateWindowSurface( window );
        }
    }

  }



  void drawCharAt(uint32_t * vbuf, int c, int r, int ch) {
    for(int cc = 0; cc<5; cc++) {
      for(int rr = 0; rr<7; rr++) {
	int ci = c * 6 + cc;
        int ri = r * 8 + rr;
        int idx = ri*width + ci;
        int value = Font5x7[(ch-32)*5 + cc] & (1<<rr);
	if(value) vbuf[idx] = ((vbuf[idx] == FG_COLOR)?BG_COLOR:FG_COLOR);
      }
    }
  }
  void drawCursor(uint32_t * vbuf) {
    if(!cursor_visible) return;
    if(!cursor_blink_state) return;

    int cbase = cursor_x * 6;
    int rbase = cursor_y * 8 + 7;
    for(int cc = 0; cc<5; cc++) {
      int idx = rbase * width + cbase + cc;
      if(idx < width*height) vbuf[idx] = FG_COLOR;
    }
  }

  void updateLCD() {
  //    0x00404040;
    uint32_t vbuf[width*height];
    uint32_t vbufScaled[width*height*4];
    for(int i = 0; i<textStart; i++) {
      for(int k = 0; k<16; k++) {
	vbuf[i*16 + k] = ((data[i] & (1<<(15-k)))?FG_COLOR:BG_COLOR);
      }
    }

    for(int col = 0; col < 53; col++) {
	for(int row = 0; row < 30; row++) {
	  int ch = data[textStart + row*53 + col];
          if(ch >= 32 && ch < 32+95) {
	    drawCharAt(vbuf, col, row, ch);
	  }
	}
    }
    drawCursor(vbuf);

    for(int x = 0; x< width; x++) {
      for(int y = 0; y< height; y++) {
        uint32_t val = vbuf[x + y*width];
        vbufScaled[x*2 + y*4*width] = val;
        vbufScaled[x*2 + y*4*width+1] = val;
        vbufScaled[x*2 + y*4*width+2*width] = val;
        vbufScaled[x*2 + y*4*width+2*width+1] = val;
      }
    }

    SDL_LockSurface(screenSurface);

    memcpy(screenSurface->pixels, vbufScaled, 4*width*height*sizeof(uint32_t));
    SDL_UnlockSurface(screenSurface);

    SDL_UpdateWindowSurface( window );

  }

  void runner() {
    int tick = 0;
    while(1) {
      updateLCD();
      usleep(10*1000);
      tick++;
      if(tick % 50 == 0) cursor_blink_state = !cursor_blink_state;
    }
  }
#endif
  std::thread updateThread;
  bool inited{false};
  int textStart{0};
 public:
  LCD(w _cmdAddr, w _dataAddr, int _width, int _height) {
    cmdAddr = _cmdAddr;
    dataAddr = _dataAddr;
    width = _width;
    height = _height;
    size = width * height / 16 + 53*30;
    textStart = width * height / 16;
    data.resize(size, 0x0000);
    cIdx = 0;
    printf("LCD: create cmd:0x%04x data:0x%04x width %d height %d\n", cmdAddr, dataAddr, width, height);
    
    data[textStart] = 'a';
        initSDL();
        updateThread = std::thread(&LCD::runner, this);
        inited = true;

  }
  virtual void terminate() {
  }
  virtual int canOperate(w addr) {
    return (addr == cmdAddr) || (addr == dataAddr);
  }
  virtual void write(w addr, w val, int seg, int force) {
    if(canOperate(addr)) {
      //printf("LCD: port 0x%04x val 0x%04x\n", addr, val);
      if(addr == cmdAddr) {
        if(val == CMD_SETADDR) {
          state = 1;
	} else if(val == CMD_SETCURSOR) {
          state = 2;
	} else if(val == CMD_CLEAR) {
	  data.clear();
          data.resize(size, 0);
	  cIdx = 0;
        }
      } else if(addr == dataAddr) {
        if(state == 1) {
          state = 0;
          cIdx = val;
        } else if(state == 2) {
          state = 3;
          cursor_x = val;
        } else if(state == 3) {
          state = 0;
          cursor_y = val;
        } else {
	  if(cIdx >= size) cIdx = 0;
	  data[cIdx] = val;
          cIdx++;
        }
      }
    }
#if SDL_SUPPORT
  //  updateLCD();
#endif
  }
  w read(w addr, int seg) {
    return 0;
  }

};


class UART : public VMemDevice {

    pthread_t worker_tid;
    bool running;

    std::deque<w> outBuffer;
    std::deque<w> inBuffer;

public:

  w addr;

  int rxIrq;
  int txIrq;
  InterruptController * intCtl;

  std::istream * in;
  std::ostream * out;

  UART(InterruptController * _intCtl, int _rxIrq, int _txIrq, w _addr, std::istream * _in, std::ostream * _out) {
    addr = _addr;

    in = _in;
    out = _out;


    intCtl = _intCtl;
    rxIrq = _rxIrq;
    txIrq = _txIrq;

    running = true;

    //start thread here
    pthread_create(&worker_tid, NULL, UART::worker_thread, this);
  }
  ~UART() {
    running = false;

    //join here
    pthread_join(worker_tid, NULL);
  }


  static void * worker_thread(void * p) {
    UART * thiz = (UART *)(p);
    while(thiz->running) {
        thiz->readLoop();
    }
    return NULL;
  }

  void readLoop() {

      int c = 0;
      if(in) {
        c = (*in).get();
      }
      w val = (w)(c);
      inBuffer.push_back(c);
  }

  virtual int canOperate(w _addr) {
    return (_addr == addr);
  }
  virtual void write(w addr, w val, int seg, int force) {
    if(canOperate(addr)) {
      if(out) {
        (*out) << (char)((char)val & 0xff);
        (*out).flush();
      }
    }
  }
  virtual w read(w addr, int seg) {
    if(canOperate(addr)) {
        if(!inBuffer.empty()) {
            w r = inBuffer.front();
            inBuffer.pop_front();
            return r;
        } else {
            return 0;
        }
    }
  }
};


class PORT : public VMemDevice {
public:
  w addr;

  std::istream * in;
  std::ostream * out;

  int readonly;
  PORT(w _addr, int _readonly, std::istream * _in, std::ostream * _out) {
    addr = _addr;
    readonly = _readonly;
    in = _in;
    out = _out;
  }

  virtual int canOperate(w _addr) {
    return (_addr == addr);
  }
  virtual void write(w addr, w val, int seg, int force) {
    if(canOperate(addr)) {
      if(out) {
        (*out) << (char)((char)val & 0xff);
        (*out).flush();
      }
    }
  }
  virtual w read(w addr, int seg) {
    if(canOperate(addr)) {
      int c = 0;
      if(in) {

        //(*in) >> c;
        if((*in).eof()) {
            c = 0;
        } else {
          c = (*in).get();
        }

        if(c) {
          printf("PORT: read %c\n", c);
        }
      }
      w val = (w)(c);
      return val;
    }
  }
};

class RAM : public VMemDevice {
  w begin;
  w size;
  w end;

  w * storage;

  int readonly;

public:
  RAM(w _begin, w _sz, int _readonly) {
    begin = _begin;
    end = _begin + _sz;
    size = _sz;
    storage = (w *)malloc(_sz * sizeof(w));
    readonly = _readonly;
  }
  ~RAM() { free(storage); }

  virtual int canOperate(w addr) { return ((addr >= begin)&&(addr<end)); }
  virtual void write(w addr, w val, int seg, int force = 0) {
    if(canOperate(addr)) {\
      if(!readonly || force) {
	     storage[addr - begin] = val;
      }
    }
  }

  virtual w read(w addr, int seg) {
    if(canOperate(addr)) {
      return storage[addr - begin];
    } else {
      return (rand() % 0xffff);
    }
  }
};

int __current_code_bank = 0;
int __current_data_bank = 0;


class ExtRAM : public VMemDevice {
  w begin;
  w size;
  w end;

  w codeBankSelector;
  w dataBankSelector;

  int cBank;
  int dBank;
  int nBanks;
  std::vector<w *> storage;



public:
  ExtRAM(w _begin, w _sz, w _codeBankSelector, w _dataBankSelector, int _nBanks) {
    begin = _begin;
    end = _begin + _sz;
    size = _sz;
    codeBankSelector = _codeBankSelector;
    dataBankSelector = _dataBankSelector;
    nBanks = _nBanks;
    cBank = 0;
    dBank = 0;
    for(int i = 0; i<nBanks; i++) {
      storage.push_back((w *)malloc(_sz * sizeof(w)));
    }
  }
  ~ExtRAM() {
    for(int i = 0; i<nBanks; i++) {
      free(storage[i]);
    }
  }

  virtual int canOperate(w addr) { return (((addr >= begin)&&(addr<end))|| (addr == codeBankSelector) || (addr == dataBankSelector)); }
  virtual void write(w addr, w val, int seg, int force = 0) {
    if(canOperate(addr)) {
      if(addr == codeBankSelector) {
        cBank = val;
	__current_code_bank = cBank;
      } else if(addr == dataBankSelector) {
        dBank = val;
	__current_data_bank = dBank;
      } else {
        //printf("ExtRAM write 0x%04x\n", val);
            if(seg) {
                storage[dBank][addr - begin] = val;
            } else {
                storage[cBank][addr - begin] = val;
            }

      }
    }
  }

  virtual w read(w addr, int seg) {
    if(canOperate(addr)) {
      if(addr == codeBankSelector) {
        return cBank;
      } else if(addr == dataBankSelector) {
        return dBank;
      } else {
          if(seg) {
            return storage[dBank][addr - begin];
          } else {
            return storage[cBank][addr - begin];
          }

      }
    }
  }
};


class ExtSegRAM : public VMemDevice {
  w begin;
  w size;
  w end;

  std::map<w, int> cBanks;

  int nBanks;
  std::vector<w *> storage;

public:
  ExtSegRAM(w _begin, w _sz, const std::vector<w> & bankSelectors, int _nBanks) {
    begin = _begin;
    end = _begin + _sz;
    size = _sz;
    nBanks = _nBanks;
    
    for(const auto b : bankSelectors) {
        cBanks[b] = 0;
    }

    for(int i = 0; i<nBanks; i++) {
      storage.push_back((w *)malloc(_sz * sizeof(w)));
    }
  }
  ~ExtSegRAM() {
    for(int i = 0; i<nBanks; i++) {
      free(storage[i]);
    }
  }

  virtual int canOperate(w addr) { return (((addr >= begin)&&(addr<end))|| 0/* addr == bankSelector */); }
  virtual void write(w addr, w val, int seg, int force = 0) {
    if(canOperate(addr)) {

        std::map<w, int>::iterator it = cBanks.find(addr);
      if(it != cBanks.end()) {
//          cBanks[addr] = val;
      } else {
        //printf("ExtRAM write 0x%04x\n", val);
//        storage[cBank][addr - begin] = val;
      }
    }
  }

  virtual w read(w addr, int seg) {
    if(canOperate(addr)) {
return 0;
        if(0/*addr == bankSelector*/) {
//        return cBank;
      } else {
//        return storage[cBank][addr - begin];
      }
    }
  }
};




class Timer : public VMemDevice {
  InterruptController * intCtl;
  unsigned long long delay;
  int irqLine;
  bool running;

  pthread_t timer_thread;


 public:
  Timer(InterruptController * _intCtl, int _irqLine, unsigned long long _delay) {
    intCtl = _intCtl;
    delay = _delay;
    irqLine = _irqLine;
    running = true;

    //start thread here
    pthread_create(&timer_thread, NULL, Timer::tick_worker, this);
  }
  ~Timer() {
    running = false;

    //join here
    pthread_join(timer_thread, NULL);
  }

  static void *tick_worker(void *timer_ptr)
  {
    Timer * thiz = (Timer *)(timer_ptr);
    while(thiz->running) {
      usleep(thiz->delay);
      thiz->tick();
    }
    return NULL;
  }

  virtual int canOperate(w addr) {return 0;}
  virtual void write(w addr, w val, int seg, int force = 0) {};
  virtual w read(w addr, int seg) {return 0;}

  void tick() {
      intCtl->request(irqLine);
  }

};


class Cpu {
  w RA, RB;
  w AP, BP, SP, T;
  size_t PC;
  w IR;
  w ML;

  w S;
  w D;

  w intEnabled;
  w userMode;

  int flDebug;

  std::vector<VMemDevice *> devices;

  InterruptController * intCtl;
public:

  w lowPC() { //bits 0-15
	return (PC&0xffff);
  }

  w mPC() {   //bits 0-(#code_block_size-1)
	return (PC&mPcMask);
  }
  w highPC() {//bits #code_block_size-(16+(#code_block_size-1))
	size_t res = PC;
	res = res / code_block_size;
	return res;
  }
  w selPC() {  //bits 16-(16+(#code_block_size-1))
	return PC>>16;
  }

  void set_mPC(w val) {
	PC = PC&(0xffff*code_block_size); 
	PC |= (val & mPcMask);
  }

  void set_highPC(w val) {
	PC = ((PC & mPcMask) | ((size_t)val * code_block_size));
  }
  void reset_mPC() {
	PC = PC&(0xffff * code_block_size);
  }

  Cpu();
  ~Cpu();
  void memWrite(w addr, w val, int seg);
  w memRead(w addr, int seg);
  void tick();
  void execute();
  void push(w val);
  w pop();
  w IRHigh();

  w seqWriterPos;
  void setSeqWriterPos(w addr) {seqWriterPos = addr;}
  void writeSeq(w val);
  void setSP(w _sp) { SP = _sp; }
  void setAP(w _ap) { AP = _ap; }
  void setBP(w _bp) { BP = _bp; }
  void setPC(w _pc) { PC = _pc; }
  void loadBin(w addr, std::string filename);

  void setDebug(int _d) {flDebug = _d;}

    w getSP() {return this->SP; }
    w getPC() {return this->PC; }

  void terminate();
};
