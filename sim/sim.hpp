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

#include <unistd.h>
#include <termios.h>


#include <pthread.h>

typedef uint16_t w;
typedef int16_t ws;


class VMemDevice {
public:
  VMemDevice() {};
  virtual ~VMemDevice() {};

  virtual int canOperate(w addr) = 0;
  virtual void write(w addr, w val, int force = 0) = 0;
  virtual w read(w addr) = 0;
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

  virtual void write(w addr, w val, int force = 0) {
    if(canOperate(addr)) {
      intVectors[addr - addrStart] = val;
    }
  }

  virtual w read(w addr) {
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
  char * data;
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
    data = (char *)(malloc(size));
    is.read(data, size);
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
    os.write(data, size);
    os.close();
  }
  virtual int canOperate(w addr) {
    return (addr == cmdAddr) || (addr == dataAddr);
  }
  virtual void write(w addr, w val, int force) {
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
              printf("HDD write 0x%04x at %d\n", val, dest);
              printf("dest : 0x%02x dest+1 : 0x%02x\n",data[dest], data[dest+1]);
              cPos+=2;
            } else {
              cState = 0;
            }
          }

        }
      }
    }
  }
  w read(w addr) {
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
              w retval = data[dest];
              retval |= ((data[dest+1])<<16);
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
  virtual void write(w addr, w val, int force) {
    if(canOperate(addr)) {
      if(out) {
        (*out) << (char)((char)val & 0xff);
        (*out).flush();
      }
    }
  }
  virtual w read(w addr) {
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
  virtual void write(w addr, w val, int force) {
    if(canOperate(addr)) {
      if(out) {
        (*out) << (char)((char)val & 0xff);
        (*out).flush();
      }
    }
  }
  virtual w read(w addr) {
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
  virtual void write(w addr, w val, int force = 0) {
    if(canOperate(addr)) {\
      if(!readonly || force) {
	     storage[addr - begin] = val;
      }
    }
  }

  virtual w read(w addr) {
    if(canOperate(addr)) {
      return storage[addr - begin];
    } else {
      return (rand() % 0xffff);
    }
  }
};


class ExtRAM : public VMemDevice {
  w begin;
  w size;
  w end;

  w bankSelector;

  int cBank;
  int nBanks;
  std::vector<w *> storage;



public:
  ExtRAM(w _begin, w _sz, w _bankSelector, int _nBanks) {
    begin = _begin;
    end = _begin + _sz;
    size = _sz;
    bankSelector = _bankSelector;
    nBanks = _nBanks;
    cBank = 0;
    for(int i = 0; i<nBanks; i++) {
      storage.push_back((w *)malloc(_sz * sizeof(w)));
    }
  }
  ~ExtRAM() {
    for(int i = 0; i<nBanks; i++) {
      free(storage[i]);
    }
  }

  virtual int canOperate(w addr) { return (((addr >= begin)&&(addr<end))|| addr == bankSelector); }
  virtual void write(w addr, w val, int force = 0) {
    if(canOperate(addr)) {
      if(addr == bankSelector) {
        cBank = val;
      } else {
        storage[cBank][addr - begin] = val;
      }
    }
  }

  virtual w read(w addr) {
    if(canOperate(addr)) {
      if(addr == bankSelector) {
        return cBank;
      } else {
        return storage[cBank][addr - begin];
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
  virtual void write(w addr, w val, int force = 0) {};
  virtual w read(w addr) {return 0;}

  void tick() {
      intCtl->request(irqLine);
  }

};


class Cpu {
  w RA, RB;
  w AP, BP, SP, T;
  w PC;
  w IR;
  w ML;


  w intEnabled;
  w userMode;

  int flDebug;

  std::vector<VMemDevice *> devices;

  InterruptController * intCtl;
public:


  Cpu();
  ~Cpu();
  void memWrite(w addr, w val);
  w memRead(w addr);
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

  void terminate();
};
