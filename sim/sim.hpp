#include <stdio.h>
#include <vector>
#include <list>
#include <map>
#include <string.h>
#include <fstream>
#include <sstream>
#include <stdint.h>
#include <stdlib.h> 
#include <math.h>
typedef uint16_t w;


class VMemDevice {
public:
  VMemDevice() {};
  virtual ~VMemDevice() {};
  
  virtual int canOperate(w addr) = 0;
  virtual void write(w addr, w val, int force = 0) = 0;
  virtual w read(w addr) = 0;
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

class Cpu {
  w RA, RB, RC; //ALU registers
  w PC, PCLatch;
  w SP, BP;
  w IR;
  
  std::vector<VMemDevice *> devices;
  
public:
  
  
  Cpu();
  void memWrite(w addr, w val);
  w memRead(w addr);
  void tick();
  
};
