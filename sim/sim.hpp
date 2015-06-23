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
#include "oplist.h"
typedef uint16_t w;
typedef int16_t ws;


class VMemDevice {
public:
  VMemDevice() {};
  virtual ~VMemDevice() {};
  
  virtual int canOperate(w addr) = 0;
  virtual void write(w addr, w val, int force = 0) = 0;
  virtual w read(w addr) = 0;
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
      if(out)
        (*out) << (char)((char)val & 0xff);
        (*out).flush();
    }
  }
  virtual w read(w addr) {
    if(canOperate(addr)) {
      char c = 0;
      if(in)
        (*in) >> c;
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

class Cpu {
  w RA, RB;
  w AP, BP, SP, T;
  w PC;
  w IR;
  w ML;
  
  int flDebug;

  std::vector<VMemDevice *> devices;
  
public:
  
  
  Cpu();
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

};
