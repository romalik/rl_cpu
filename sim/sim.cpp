#include "sim.hpp"
#include <iostream>
#include <stdlib.h>
#include <unistd.h>
Cpu::Cpu() {
//test config, full ram memory, last 2 words - in/output

    flDebug = 0;

  this->devices.push_back(new RAM(0, 0x8000, 0));

  this->devices.push_back(new PORT(0xffff, 0, NULL, &std::cout));

}

void Cpu::memWrite(w addr, w val) {
    for(int i = 0; i<this->devices.size(); i++) {
      if(this->devices[i]->canOperate(addr)) {
        this->devices[i]->write(addr,val);
        if(flDebug) printf("memWrite [0x%04x] : 0x%04x\n", addr, val);
      }
    }
}

w Cpu::memRead(w addr) {
    for(int i = 0; i<this->devices.size(); i++) {
      if(this->devices[i]->canOperate(addr)) {
          w res = this->devices[i]->read(addr);
          //if(flDebug) printf("memRead [0x%04x] : 0x%04x\n", addr, res);
          return res;
      }
    }
}

void Cpu::tick() {
  //insert delay here
    //100 kHz
    usleep(10);
}

void Cpu::push(w val) {
    tick();
    memWrite(SP, val);
    SP++;
    tick();
}

w Cpu::pop() {
    tick();
    SP--;
    w res = memRead(SP);
    tick();
    return res;
}

w Cpu::IRHigh() {
    return ((IR >> 8)&0xff);
}

void Cpu::execute() {
  this->IR = this->memRead(PC);
  if(flDebug) {

      printf("PC: 0x%04X, IR: 0x%04X ('%s')\n", PC, IR, oplist[IR&0xff]);
      printf("Stack: ");
      for(int i = 0; i<10; i++) {
          w val =  memRead(SP + i - 9);
         printf("0x%04X(%c) ", val, (val > 32 && val < 127)?(char)val:'-');
      }
      printf("\n");
  }

  tick();
  this->PC++;
  
  unsigned char op = this->IR & 0xff;

  if(op == nop){
      //this is nop, come on
  } else if(op == addrf_b) {
    this->push(this->AP + IRHigh());
  } else if(op == addrf_w) {
      w tmp = this->memRead(PC);
      this->PC++;
      this->push(this->AP + tmp);
  } else if(op == iaddrf_b) {
    this->push(memRead(this->AP + IRHigh()));
  } else if(op == iaddrf_w) {
      w tmp = this->memRead(PC);
      this->PC++;
      this->push(memRead(this->AP + tmp));


  } else if(op == addrl_b) {
    this->push(this->BP + IRHigh());
  } else if(op == addrl_w) {
      w tmp = this->memRead(PC);
      this->PC++;
      this->push(this->BP + tmp);
  } else if(op == iaddrl_b) {
    this->push(memRead(this->BP + IRHigh()));
  } else if(op == iaddrl_w) {
      w tmp = this->memRead(PC);
      this->PC++;
      this->push(memRead(this->BP + tmp));


  } else if(op == cnst_b) {
    this->push(IRHigh());
  } else if(op == cnst_w) {
      w tmp = this->memRead(PC);
      this->PC++;
      this->push(tmp);
  } else if(op == icnst_b) {
    this->push(memRead(IRHigh()));
  } else if(op == icnst_w) {
      w tmp = this->memRead(PC);
      this->PC++;
      this->push(memRead(tmp));


  } else if(op == addrs_b) {
    this->push(this->SP + IRHigh());
  } else if(op == addrs_w) {
      w tmp = this->memRead(PC);
      this->PC++;
      this->push(this->SP + tmp);
  } else if(op == iaddrs_b) {
    this->push(memRead(this->SP + IRHigh()));
  } else if(op == iaddrs_w) {
      w tmp = this->memRead(PC);
      this->PC++;
      this->push(memRead(this->SP + tmp));


  } else if(op == indir) {
    this->push(memRead(this->pop()));


  } else if(op == add) {
    push(pop()+pop());
  } else if(op == add_b) {
    push(pop()+IRHigh());
  } else if(op == add_w) {
      w tmp = this->memRead(PC);
      this->PC++;
    push(pop()+tmp);


  } else if(op == band) {
    push(pop()&pop());
  } else if(op == band_b) {
    push(pop()&IRHigh());
  } else if(op == band_w) {
      w tmp = this->memRead(PC);
      this->PC++;
    push(pop()&tmp);


  } else if(op == bor) {
    push(pop()|pop());
  } else if(op == bor_b) {
    push(pop()|IRHigh());
  } else if(op == bor_w) {
      w tmp = this->memRead(PC);
      this->PC++;
    push(pop()|tmp);


  } else if(op == bxor) {
    push(pop()^pop());
  } else if(op == bxor_b) {
    push(pop()^IRHigh());
  } else if(op == bxor_w) {
      w tmp = this->memRead(PC);
      this->PC++;
    push(pop()^tmp);


  } else if(op == lsh) {
    push(pop()<<pop());
  } else if(op == lsh_b) {
    push(pop()<<IRHigh());
  } else if(op == lsh_w) {
      w tmp = this->memRead(PC);
      this->PC++;
    push(pop()<<tmp);


  } else if(op == rsh) {
    push(pop()>>pop());
  } else if(op == rsh_b) {
    push(pop()>>IRHigh());
  } else if(op == rsh_w) {
      w tmp = this->memRead(PC);
      this->PC++;
    push(pop()>>tmp);


  } else if(op == sub) {
    push(pop()-pop());
  } else if(op == sub_b) {
    push(pop()-IRHigh());
  } else if(op == sub_w) {
      w tmp = this->memRead(PC);
      this->PC++;
    push(pop()-tmp);



  } else if(op == eq_w) {
    int v2 = (int)pop();
    int v1 = (int)pop();
    w addr = this->memRead(PC);
    PC++;
    if(v1 == v2) {
        PC = addr;
    }

  } else if(op == ge_w) {
    int v2 = (int)pop();
    int v1 = (int)pop();
    w addr = this->memRead(PC);
    PC++;
    if(v1 >= v2) {
        PC = addr;
    }

  } else if(op == gt_w) {
    int v2 = (int)pop();
    int v1 = (int)pop();
    w addr = this->memRead(PC);
    PC++;
    if(v1 > v2) {
        PC = addr;
    }

  } else if(op == le_w) {
    int v2 = (int)pop();
    int v1 = (int)pop();
    w addr = this->memRead(PC);
    PC++;
    if(v1 <= v2) {
        PC = addr;
    }

  } else if(op == lt_w) {
    int v2 = (int)pop();
    int v1 = (int)pop();
    w addr = this->memRead(PC);
    PC++;
    if(v1 < v2) {
        PC = addr;
    }

  } else if(op == ne_w) {
    int v2 = (int)pop();
    int v1 = (int)pop();
    w addr = this->memRead(PC);
    PC++;
    if(v1 != v2) {
        PC = addr;
    }

  } else if(op == ueq_w) {
    w v2 = pop();
    w v1 = pop();
    w addr = this->memRead(PC);
    PC++;
    if(v1 == v2) {
        PC = addr;
    }

  } else if(op == uge_w) {
      w v2 = pop();
      w v1 = pop();
    w addr = this->memRead(PC);
    PC++;
    if(v1 >= v2) {
        PC = addr;
    }

  } else if(op == ugt_w) {
      w v2 = pop();
      w v1 = pop();
    w addr = this->memRead(PC);
    PC++;
    if(v1 > v2) {
        PC = addr;
    }

  } else if(op == ule_w) {
      w v2 = pop();
      w v1 = pop();
    w addr = this->memRead(PC);
    PC++;
    if(v1 <= v2) {
        PC = addr;
    }

  } else if(op == ult_w) {
      w v2 = pop();
      w v1 = pop();
    w addr = this->memRead(PC);
    PC++;
    if(v1 < v2) {
        PC = addr;
    }

  } else if(op == une_w) {
      w v2 = pop();
      w v1 = pop();
    w addr = this->memRead(PC);
    PC++;
    if(v1 != v2) {
        PC = addr;
    }

  } else if(op == call) {
    w target = pop();
    push(PC);
    push(AP);
    push(BP);
    AP = BP;
    BP = SP;
    PC = target;

  } else if(op == call_w) {
    w target = memRead(PC);
    PC++;
    push(PC);
    push(AP);
    push(BP);
    AP = BP;
    BP = SP;
    PC = target;

  } else if(op == ret) {
    RA = pop();
    SP = BP;
    BP = pop();
    AP = pop();
    w target = pop();
    push(RA);
    PC = target;
  } else if(op == ret2) {
      RA = pop();
      RB = pop();
    SP = BP;
    BP = pop();
    AP = pop();
    w target = pop();
    push(RB);
    push(RA);
    PC = target;

  } else if(op == jump_w) {
      w target = memRead(PC);
      PC++;
      PC = target;
  } else if(op == jump) {
      w target = pop();
      PC = target;
  } else if(op == discard1) {
      pop();
  } else if(op == discard_b) {
      SP -= IRHigh();
  } else if(op == discard_w) {
      w discardAmount = memRead(PC);
      PC++;
      SP -= discardAmount;
  } else if(op == alloc1) {
      SP += 1;
  } else if(op == alloc_b) {
      SP += IRHigh();
  } else if(op == alloc_w) {
      w allocAmount = memRead(PC);
      PC++;
      SP += allocAmount;
  } else if(op == store) {
      w val = pop();
      w target = pop();
      memWrite(target, val);
  } else if(op == rstore) {
      w target = pop();
      w val = pop();
      memWrite(target, val);
  } else if(op == fastcall_w) {
      w target = memRead(PC);
      PC++;
      push(PC);
      PC = target;

  } else if(op == fastcall) {
      w target = pop();
      push(PC);
      PC = target;

  } else if(op == fastret) {
      w retval = pop();
      w target = pop();
      push(retval);
      PC = target;
  } else if(op == fastret2) {
      RA = pop();
      RB = pop();
      w target = pop();
      push(RB);
      push(RA);
      PC = target;

  }





}


void Cpu::writeSeq(w val) {
    memWrite(seqWriterPos, val);
    seqWriterPos++;
}

void Cpu::loadBin(w addr, std::string filename) {
    std::ifstream file;
    file.open(filename.c_str(), std::ios::in | std::ios::binary);
    char image[0xffff*2];
    file.read(image, 0xffff*2);
    setSeqWriterPos(addr);
    for(int i = 0; i<=0xffff; i++) {
        writeSeq(((w)((uint8_t)image[i*2]) << 8) | (uint8_t)image[i*2 + 1] );
    }
}

int test() {
    Cpu myCpu;
    myCpu.memWrite(0x2010, 0x40);
    w val = myCpu.memRead(0x2010);
    printf("read 0x2010 val (should be 0x40): 0x%04X\n", val);

    printf("now write H to 0xffff: ");

    myCpu.memWrite(0xffff, 'H');

    printf("\nCool, it works!\n");
    printf("Writing test program...\n");
    myCpu.setSeqWriterPos(0);

    myCpu.writeSeq(cnst_w);
    myCpu.writeSeq(0xffff);
    myCpu.writeSeq(cnst_w);
    myCpu.writeSeq('A');
    myCpu.writeSeq(store);

    printf("Writing done\n");

    myCpu.setPC(0);
    myCpu.setSP(0x1000);

    printf("Running program, should write 'A'\n");
    myCpu.execute();
    myCpu.execute();
    myCpu.execute();
    printf("\n");
    return 0;

}

int main(int argc, char ** argv) {
    if(argc < 2)
        return 0;

    if(!strcmp(argv[1], "-test")) {
        test();
    } else {
        int debug = 0;
        if(argc > 2) {
            if(!strcmp(argv[2], "-d")) {
                debug = 1;
            }
        }

        printf("Loading binary %s...\n", argv[1]);
        Cpu myCpu;
        myCpu.setDebug(debug);
        myCpu.setPC(0);
        myCpu.setSP(0x4000);
        myCpu.setBP(0x4000);
        myCpu.loadBin(0,std::string(argv[1]));
        printf("\nLoading done. Starting..\n");
        while(1) {
            myCpu.execute();
            if(debug)
                usleep(500*1000);
        }


    }
    return 0;
}
