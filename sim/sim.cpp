#include "sim.hpp"

Cpu::Cpu() {
  this->devices.push_back(new RAM(0, 0x2000, 1));
  this->devices.push_back(new RAM(0x2000, 0x2000, 0));
}

void Cpu::memWrite(w addr, w val) {
    for(int i = 0; i<this->devices.size(); i++) {
      if(this->devices[i]->canOperate(addr)) {
	this->devices[i]->write(addr,val);
      }
    }
}

w Cpu::memRead(w addr) {
    for(int i = 0; i<this->devices.size(); i++) {
      if(this->devices[i]->canOperate(addr)) {
	return this->devices[i]->read(addr);
      }
    }
}


int main() {
  Cpu myCpu;
  myCpu.memWrite(0x10, 0x20);
  myCpu.memWrite(0x2010, 0x40);
  w val = myCpu.memRead(0x10);
  printf("read 0x10 val: 0x%04X\n", val);
  val = myCpu.memRead(0x2010);
  printf("read 0x2010 val (should be 0x40): 0x%04X\n", val);
    return 0;
}