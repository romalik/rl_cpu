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
#include <functional>

#include <unistd.h>
#include <termios.h>
#include <mutex>
#include <thread>
#include <pthread.h>
#define SDL_SUPPORT 1

/* interrupt

  push PC
  push BP
  push AP
  push S
  push D
  push SW

  jmp to vec
  */
/*
  reti

  pop SP & pop MMUSelector
  pop SW
  pop D
  pop S
  pop AP
  pop BP
  pop SW
  pop PC

*/

const int code_block_size = 8;
const int mPcMask = code_block_size-1;
typedef uint16_t w;
typedef int16_t ws;

#define ADDR_CODE_FLAG (1 << 19)
#define ADDR_IO_FLAG (1 << 20)

#define ADDR_PAGE_OFFSET_MASK (0xfff)
#define ADDR_PAGE_NUM_MASK (0x7f000)
#define ADDR_PAGE_NUM_SHIFT (12)

#define MMU_PROCESS_ENTRY_SIZE (8)
#define MMU_TABLE_SELECT_BIT (1 << 14)

#define MMU_TABLE_SELECTOR_ADDR 0x00


#define IO_ADDR_SELECTOR_MASK 0x40e0
#define IO_ADDR_SELECTOR_SHIFT 5

#define IO_ADDR_MMU_SELECTOR_RANGE 0
#define IO_ADDR_INTERRUPT_CONTROLLER_RANGE 1
#define IO_ADDR_UART_RANGE 2
#define IO_ADDR_ATA_RANGE 3
#define IO_ADDR_GLCD_RANGE 4
#define IO_ADDR_GPIO_RANGE 5


bool addrIsIO(size_t a) {
    return a & ADDR_IO_FLAG;
}


int getIoRange(size_t a) {
    return ((a & IO_ADDR_SELECTOR_MASK) >> IO_ADDR_SELECTOR_SHIFT);
}

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

class Cpu; //forward

class VMemDevice {
    Cpu * cpu;
public:
    VMemDevice(Cpu * _cpu = NULL);
    virtual ~VMemDevice() {};
    void regInCPU(std::function<bool(size_t)>, std::function<size_t(size_t)>);
    void regMMUTableInCPU();
    void regRamInCPU(std::function<bool(size_t)>, std::function<size_t(size_t)>);
    virtual void write(size_t addr, w val, int seg, int force = 0) = 0;
    virtual w read(size_t addr, int seg) = 0;
    virtual void terminate() {};
};

class InterruptController : public VMemDevice {
    int currentInterrupt;
    w pendingInterrupts;
    int nIRQs;
    std::vector<w> intVectors;
public:
    InterruptController(int _nIRQs) {
        nIRQs = _nIRQs;
        intVectors.resize(nIRQs, 0);
        currentInterrupt = 0;
        pendingInterrupts = 0;

        auto selectFn = [this](size_t a) -> bool {
            return getIoRange(a) == IO_ADDR_INTERRUPT_CONTROLLER_RANGE;
        };

        auto transformFn = [this](size_t a) -> size_t {
            return (a&0x1f);
        };
        regInCPU(selectFn, transformFn);

    }
    ~InterruptController() {}


    virtual void write(size_t addr, w val, int seg, int force = 0) {
        printf("Write to int vector 0x%08X\n", addr);
        intVectors[addr] = val;
    }

    virtual w read(size_t addr, int seg) {
        return intVectors[addr];
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
    HDD(std::string path) {
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
        //    cmdAddr = _cmdAddr;
        //    dataAddr = _dataAddr;
        cmdAddr = 0x01;
        dataAddr = 0x00;
        printf("HDD: create cmd:0x%04x data:0x%04x\n", cmdAddr, dataAddr);

        auto selectFn = [this](size_t a) -> bool {
            return getIoRange(a) == IO_ADDR_ATA_RANGE;
        };

        auto transformFn = [this](size_t a) -> size_t {
            return (a&0x1f);
        };
        regInCPU(selectFn, transformFn);

    }
    virtual void terminate() {
        std::ofstream os(image_path.c_str(), std::ofstream::binary);
        os.write((char *)data, size);
        os.close();
    }
    virtual void write(size_t addr, w val, int seg, int force) {
        if(addr == 0) {
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
        } else if(addr == 1) {
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
    w read(size_t addr, int seg) {
        if(addr == 0) {
            return 0;
        } else if(addr == 1) {
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
    LCD(int _width, int _height) {
        //    cmdAddr = _cmdAddr;
        //    dataAddr = _dataAddr;
        cmdAddr = 0x01;
        dataAddr = 0x00;
        width = _width;
        height = _height;
        size = width * height / 16 + 53*30;
        textStart = width * height / 16;
        data.resize(size, 0x0000);
        cIdx = 0;
        printf("LCD: create cmd:0x%04x data:0x%04x width %d height %d\n", cmdAddr, dataAddr, width, height);
        auto selectFn = [this](size_t a) -> bool {
            return getIoRange(a) == IO_ADDR_GLCD_RANGE;
        };

        auto transformFn = [this](size_t a) -> size_t {
            return (a&0x1f);
        };
        regInCPU(selectFn, transformFn);

        data[textStart] = 'a';
        initSDL();
        updateThread = std::thread(&LCD::runner, this);
        inited = true;

    }
    virtual void terminate() {
    }
    virtual void write(size_t addr, w val, int seg, int force) {
        //printf("LCD: port 0x%04x val 0x%04x\n", addr, val);
        if(addr == 0) {
            if(val == CMD_SETADDR) {
                state = 1;
            } else if(val == CMD_SETCURSOR) {
                state = 2;
            } else if(val == CMD_CLEAR) {
                data.clear();
                data.resize(size, 0);
                cIdx = 0;
            }
        } else if(addr == 1) {
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
#if SDL_SUPPORT
        //  updateLCD();
#endif
    }
    w read(size_t addr, int seg) {
        return 0;
    }

};

class MMUTable : public VMemDevice {
    size_t size;
public:

    std::vector<size_t> table;


    MMUTable() {
        size = 32768;

        table.resize(size, 0xDEADDEAD);

        /*
    for(size_t i = 0; i<16; i++) {
        table[i] = i;
        table[i|(1<<12)] = i;

        printf("init MMU 0x%08X : 0x%08X\n", i, i);
        printf("init MMU 0x%08X : 0x%08X\n", i|(1<<12), i);
    }
*/
        auto selectFn = [this](size_t a) -> bool {
            return (a & MMU_TABLE_SELECT_BIT);
        };

        auto transformFn = [](size_t a) -> size_t {
            return a & (MMU_TABLE_SELECT_BIT-1);
        };
        regInCPU(selectFn, transformFn);
        regMMUTableInCPU();
    }
    ~MMUTable() {
    }


    virtual void write(size_t addr, w val, int seg, int force) {
        table[addr] = val;
        //	printf("MMUTable write 0x%08X : 0x%08X\n", addr, val);
    }
    virtual w read(size_t addr, int seg) {
        return 0;
    }

    size_t getRealAddress(size_t a, w processSelector) {
        size_t page_entry_address = (((a & ADDR_PAGE_NUM_MASK) >> (ADDR_PAGE_NUM_SHIFT)) | ((size_t)processSelector << MMU_PROCESS_ENTRY_SIZE) | (((a & ADDR_CODE_FLAG)?1:0) << 12));
        if(page_entry_address < size) {
            size_t res = ((a & ADDR_PAGE_OFFSET_MASK) | (table[page_entry_address] << ADDR_PAGE_NUM_SHIFT));

            //	printf("MMU selector %d entryAddress 0x%08X : 0x%08X -> 0x%08X\n", processSelector, page_entry_address, a, res);
            return res;
        } else {
            printf("MMU FAULT! Request address 0x%08X, processSelector 0x%08X\nExiting.\n", a, (size_t)processSelector); exit(1);
        }
    }
};




class UART : public VMemDevice {

    pthread_t worker_tid;
    bool running;

    std::deque<w> outBuffer;
    std::deque<w> inBuffer;

public:

    size_t addr;

    int rxIrq;
    int txIrq;
    InterruptController * intCtl;

    std::istream * in;
    std::ostream * out;

    UART(InterruptController * _intCtl, int _rxIrq, int _txIrq, std::istream * _in, std::ostream * _out) {
        //    addr = _addr;
        addr = 0x00;

        in = _in;
        out = _out;


        intCtl = _intCtl;
        rxIrq = _rxIrq;
        txIrq = _txIrq;

        running = true;
        auto selectFn = [this](size_t a) -> bool {
            return getIoRange(a) == IO_ADDR_UART_RANGE;
        };

        auto transformFn = [this](size_t a) -> size_t {
            return (a&0x1f);
        };
        regInCPU(selectFn, transformFn);

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

    virtual void write(size_t addr, w val, int seg, int force) {
        if(out) {
            (*out) << (char)((char)val & 0xff);
            (*out).flush();
        }
    }
    virtual w read(size_t addr, int seg) {
        if(!inBuffer.empty()) {
            w r = inBuffer.front();
            inBuffer.pop_front();
            return r;
        } else {
            return 0;
        }
    }
};


class PORT : public VMemDevice {
public:
    size_t addr;

    std::istream * in;
    std::ostream * out;

    int readonly;
    PORT(int _readonly, std::istream * _in, std::ostream * _out) {
        //    addr = _addr;
        addr = 0x00;
        readonly = _readonly;
        in = _in;
        out = _out;
        auto selectFn = [this](size_t a) -> bool {
            return getIoRange(a) == IO_ADDR_GPIO_RANGE;
        };

        auto transformFn = [this](size_t a) -> size_t {
            return (a&0x1f);
        };
        regInCPU(selectFn, transformFn);
    }

    virtual void write(size_t addr, w val, int seg, int force) {
        if(out) {
            (*out) << (char)((char)val & 0xff);
            (*out).flush();
        }
    }
    virtual w read(size_t addr, int seg) {
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
};

class RAM : public VMemDevice {
    size_t begin;
    size_t size;
    size_t end;

    std::vector<w> storage;

public:
    RAM(size_t _begin, size_t _sz) {
        begin = _begin;
        end = _begin + _sz;
        size = _sz;
        storage.resize(size, 0);
        auto selectFn = [this](size_t a) -> bool {
            //selects when io flag not active
            return (!(a & (1<<20)));
        };

        auto transformFn = [](size_t a) -> size_t {
            return a;
        };
        regRamInCPU(selectFn, transformFn);
    }
    ~RAM() { }

    virtual void write(size_t addr, w val, int seg, int force = 0) {
        storage[addr - begin] = val;
    }

    virtual w read(size_t addr, int seg) {
        return storage[addr - begin];
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

    virtual void write(size_t addr, w val, int seg, int force = 0) {};
    virtual w read(size_t addr, int seg) {return 0;}

    void tick() {
        intCtl->request(irqLine);
    }

};

struct DemuxEntry {
    DemuxEntry(VMemDevice * _dev, std::function<bool(size_t)> _selectFunc, std::function<size_t(size_t)> _transformFunc) : dev(_dev), selectFunc(_selectFunc), transformFunc(_transformFunc) {};

    VMemDevice * dev;
    std::function<bool(size_t)> selectFunc;
    std::function<size_t(size_t)> transformFunc;

    bool operator()(size_t a) const { return selectFunc(a); }
};

class Demux {
    std::vector<DemuxEntry> devs;
    std::vector<DemuxEntry> rams;
    MMUTable * mmuTable;
public:
    Demux() {};
    void regMMUTable(MMUTable * tab) {
        mmuTable = tab;
    }
    void regDevice(VMemDevice * dev, std::function<bool(size_t)> selectFunc, std::function<size_t(size_t)> transformFunc) {
        devs.push_back(DemuxEntry(dev, selectFunc, transformFunc));
    }
    void regRam(VMemDevice * dev, std::function<bool(size_t)> selectFunc, std::function<size_t(size_t)> transformFunc) {
        rams.push_back(DemuxEntry(dev, selectFunc, transformFunc));
    }
    void memWrite(size_t effAddr, w val, w mmuSelector, w mmuEnabled) {
        if(effAddr & (1<<20)) { //this is io or mmu
            //      if(effAddr & (1<<14)) { //this is mmu
            //      effAddr &= 0x3fff;

            //write pagetable here
            //	mmuTable->table[effAddr] = val;
            //	printf("write mmu table 0x%08X : 0x%08X\n\n", effAddr, val);
            //      } else { //this is io
            //write io here
            //	  effAddr &= 0x1ff;
            effAddr &= 0xffff;
            for(const auto & dev : devs) {
                if(dev(effAddr)) {
                    return dev.dev->write(dev.transformFunc(effAddr), val, 0, 0);
                }
            }

            //      }
        } else { //this is ram
            size_t realAddr = 0x00; //get real address from mmu process selector->pagetable
            //write realaddress to ram here
            if(!mmuEnabled) {
                effAddr &= 0xffff;
            } else {
                effAddr = mmuTable->getRealAddress(effAddr, mmuSelector);
                //get real address
            }
            for(const auto & ram : rams) {
                if(ram(effAddr)) {
                    return ram.dev->write(ram.transformFunc(effAddr), val, 0, 0);
                }
            }
        }

        //printf("WRITE: Device for address 0x%08X not found!\n", effAddr);
    }
    w memRead(size_t effAddr, w mmuSelector, w mmuEnabled) {
        if(effAddr & (1<<20)) { //this is io or mmu
            //      if(effAddr & (1<<14)) { //this is mmu
            //	effAddr &= 0x3fff;
            //	return mmuTable->table[effAddr];
            //     } else { //this is io
            //write io here
            //	effAddr &= 0x1ff;
            effAddr &= 0xffff;
            for(const auto & dev : devs) {
                if(dev(effAddr)) {
                    return dev.dev->read(dev.transformFunc(effAddr), 0);
                }
            }

            //      }
        } else { //this is ram
            size_t realAddr = 0x00; //get real address from mmu process selector->pagetable
            if(!mmuEnabled) {
                effAddr &= 0xffff;
            } else {
                //get real address
                effAddr = mmuTable->getRealAddress(effAddr, mmuSelector);
            }
            //write realaddress to ram here
            for(const auto & ram : rams) {
                if(ram(effAddr)) {
                    return ram.dev->read(ram.transformFunc(effAddr), 0);
                }
            }
        }
        printf("READ: Device for address 0x%08X not found!\n", effAddr);
        return 0;
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

    w MMUEnabled;

    w MMUEntrySelector;

    int flDebug;

    std::vector<VMemDevice *> devices;


    InterruptController * intCtl;
public:

    Demux demux;

    void regDevice(VMemDevice * dev, std::function<bool(size_t)> selectFunc, std::function<size_t(size_t)> transformFunc) {
        printf("Reg device in demux\n");
        demux.regDevice(dev, selectFunc, transformFunc);
    }
    void regRam(VMemDevice * dev, std::function<bool(size_t)> selectFunc, std::function<size_t(size_t)> transformFunc) {
        printf("Reg device in demux\n");
        demux.regRam(dev, selectFunc, transformFunc);
    }


    void dumpRegs() {
        printf("PC 0x%08X\nRA 0x%08X\nRB 0x%08X\nAP 0x%08X\nBP 0x%08X\nSP 0x%08X\nT 0x%08X\nIR 0x%08X\nML 0x%08X\nS 0x%08X\nD 0x%08X\bint %d\nuser %d\nMMU %d\n", PC, RA, RB, AP, BP, SP, T, IR, ML, S, D, intEnabled, userMode, MMUEnabled);
    }

    w SW() {
        //15 14    13    12     11 10 09 08 07 06 05 04 03 02 01 00
        //-- MMUEn User  IntEn  ----mPC---- ------mmuSelector------

        w res = 0;
        res |= (MMUEntrySelector & 0xff);
        res |= ((mPC()) << 8);
        res |= (intEnabled << 12);
        res |= (userMode << 13);
        //	res |= (MMUEnabled << 14);
        return res;
    }


    void set_SW(w sw) {
        //    MMUEnabled = sw & (1<<14);
        userMode   = (sw & (1<<13))?1:0;
        intEnabled = (sw & (1<<12))?1:0;
        set_mPC((sw & (mPcMask<<8)) >> 8);
        MMUEntrySelector = sw & 0xff;

    }
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
    void memWrite(size_t addr, w val, int seg);
    w memRead(size_t addr, int seg);
    void tick();
    void execute();
    void push(w val);
    w pop();
    w IRHigh();

    w seqWriterPos;
    void setSeqWriterPos(size_t addr) {seqWriterPos = addr;}
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
