#include "sim.hpp"
#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/time.h>
#include <fcntl.h>
#include <time.h>
#ifdef __MACH__
#include <mach/clock.h>
#include <mach/mach.h>
#endif

long long gettime_ms() {
    struct timeval te;
    gettimeofday(&te, NULL);
    long long milliseconds = te.tv_sec*1000LL + te.tv_usec/1000;
    return milliseconds;
}


Cpu myCpu;
struct termios old_tio, new_tio;
int oldf;


Cpu::Cpu() {
//test config, full ram memory, last 2 words - in/output

    flDebug = 0;

  intEnabled = 0;
  userMode = 0;

  intCtl = new InterruptController(0x7fea, 8);


  this->devices.push_back(new RAM(0, 16*1024, 0)); //ROM
  this->devices.push_back(new RAM(0x4000, 16*1024-16, 0)); //RAM

//  this->devices.push_back(new RAM(0x8000, 32*1024-1, 0)); //RAM
  this->devices.push_back(new ExtRAM(0x8000, 32*1024-1, 0x7fdf, 16)); //ExtRAM

  this->devices.push_back(new UART(NULL,0,0, 0x7ffe, &std::cin, NULL));
  this->devices.push_back(new PORT(0x7fff, 0, NULL, &std::cout));

  this->devices.push_back(new HDD(0x7ffc, 0x7ffd, std::string("hdd")));
  this->devices.push_back(new Timer(intCtl, 3, 50000ULL));
  this->devices.push_back(intCtl);

}






Cpu::~Cpu() {
}

w SP_min;
w SP_max;

long long totalInstr = 0;
long long startTime = 0;

void Cpu::terminate() {

  long long cTime = gettime_ms();
  int dT = cTime - startTime;
  long ipsActual = totalInstr/(static_cast<float>(dT)/1000.0f);

  printf("CPU terminating\n");


  for(int i = 0; i<this->devices.size(); i++) {
    this->devices[i]->terminate();
  }

  printf("SP_min: 0x%04x\nSP_max 0x%04x\n", SP_min, SP_max);
  printf("Average ips %d\n", ipsActual);

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
    //1000 kHz
    //usleep(1);
    //

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
    w t = ((IR >> 8)&0xff);
    if(t & 0x80) { //sign-extend
        t = t | 0xff00;
    }
    return t;
}

void Cpu::execute() {
  if(intEnabled) {
//    printf("CPU: IRQ Line Status: %d\n", intCtl->irqLineStatus());
    if(intCtl->irqLineStatus()) {
      //printf("Into int vec!\n");
      this->push(SP);
      this->push(PC);
      this->push(BP);
      this->push(AP);
      this->userMode = 0;
      //printf("Try get vec..");
      PC = intCtl->getIrqVector();
      //printf(" 0x%04x\n", PC);
    }
  }


  this->IR = this->memRead(PC);
  if(flDebug /* || (IR&0xff) == ret2|| (IR&0xff) == le_w|| (IR&0xff) == lt_w */) {

      printf("PC: 0x%04X, IR: 0x%04X ('%s')\n", PC, IR, oplist[IR&0xff]);
      printf("Stack: ");
      for(int i = 0; i<10; i++) {
          w val =  memRead(SP + i - 10);
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

  } else if(op == indir2) {


      w tmp = this->pop();
      this->push(memRead(tmp));
      this->push(memRead(tmp+1));


  } else if(op == add) {
    push(pop()+pop());
  } else if(op == add_b) {
    push(pop()+IRHigh());
  } else if(op == add_w) {
      w tmp = this->memRead(PC);
      this->PC++;
    push(pop()+tmp);
  } else if(op == add2) {
      w ah = pop();
      w al = pop();
      w bh = pop();
      w bl = pop();
      long res = (((long)ah<<16) | (long)al) + (((long)bh<<16) | (long)bl);
      push(res&0xffff);
      push((res>>16)&0xffff);


  } else if(op == band) {
    push(pop()&pop());
  } else if(op == band_b) {
    push(pop()&IRHigh());
  } else if(op == band_w) {
      w tmp = this->memRead(PC);
      this->PC++;
    push(pop()&tmp);
  } else if(op == band2) {
      w ah = pop();
      w al = pop();
      w bh = pop();
      w bl = pop();
      long res = (((long)ah<<16) | (long)al) & (((long)bh<<16) | (long)bl);
      push(res&0xffff);
      push((res>>16)&0xffff);


  } else if(op == bor) {
    push(pop()|pop());
  } else if(op == bor_b) {
    push(pop()|IRHigh());
  } else if(op == bor_w) {
      w tmp = this->memRead(PC);
      this->PC++;
    push(pop()|tmp);
  } else if(op == bor2) {
      w ah = pop();
      w al = pop();
      w bh = pop();
      w bl = pop();
      long res = (((long)ah<<16) | (long)al) | (((long)bh<<16) | (long)bl);
      push(res&0xffff);
      push((res>>16)&0xffff);


  } else if(op == bxor) {
    push(pop()^pop());
  } else if(op == bxor_b) {
    push(pop()^IRHigh());
  } else if(op == bxor_w) {
      w tmp = this->memRead(PC);
      this->PC++;
    push(pop()^tmp);
  } else if(op == bxor2) {
      w ah = pop();
      w al = pop();
      w bh = pop();
      w bl = pop();
      long res = (((long)ah<<16) | (long)al) ^ (((long)bh<<16) | (long)bl);
      push(res&0xffff);
      push((res>>16)&0xffff);


  } else if(op == lsh) {
    push(pop() << 1);
  } else if(op == rsh) {
      push(pop() >> 1);


  } else if(op == sub) {
      //BUG!
        RA = pop();
        RB = pop();
      push(RB - RA);
  } else if(op == sub_b) {
    push(pop()-IRHigh());
  } else if(op == sub_w) {
      w tmp = this->memRead(PC);
      this->PC++;
    push(pop()-tmp);
  } else if(op == sub2) {
      w ah = pop();
      w al = pop();
      w bh = pop();
      w bl = pop();
      long res = (((long)bh<<16) | (long)bl) - (((long)ah<<16) | (long)al);
      push(res&0xffff);
      push((res>>16)&0xffff);
  } else if(op == eq_w) {
    int v2 = (ws)pop();
    int v1 = (ws)pop();
    w addr = this->memRead(PC);
    PC++;
    if(v1 == v2) {
        PC = addr;
    }

  } else if(op == ge_w) {
    int v2 = (ws)pop();
    int v1 = (ws)pop();
    w addr = this->memRead(PC);
    PC++;
    if(v1 >= v2) {
        PC = addr;
    }

  } else if(op == gt_w) {
    int v2 = (ws)pop();
    int v1 = (ws)pop();
    w addr = this->memRead(PC);
    PC++;
    if(v1 > v2) {
        PC = addr;
    }

  } else if(op == le_w) {
    int v2 = (ws)pop();
    int v1 = (ws)pop();
    w addr = this->memRead(PC);
    PC++;
    if(v1 <= v2) {
        PC = addr;
    }

  } else if(op == lt_w) {
    int v2 = (ws)pop();
    int v1 = (ws)pop();
    w addr = this->memRead(PC);
    PC++;
    if(v1 < v2) {
        PC = addr;
    }

  } else if(op == ne_w) {
    int v2 = (ws)pop();
    int v1 = (ws)pop();
    w addr = this->memRead(PC);
    PC++;
    if(v1 != v2) {
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

  } else if(op == call0) {
    w target = pop();
    push(PC);
    push(AP);
    push(BP);
    AP = BP;
    BP = SP;
    PC = target;

  } else if(op == call0_w) {
    w target = memRead(PC);
    PC++;
    push(PC);
    push(AP);
    push(BP);
    AP = BP;
    BP = SP;
    PC = target;

  } else if(op == call1) {
    w target = pop();
    SP++;
    push(PC);
    push(AP);
    push(BP);
    AP = BP;
    BP = SP;
    PC = target;

  } else if(op == call1_w) {
    w target = memRead(PC);
    PC++;
    SP++;
    push(PC);
    push(AP);
    push(BP);
    AP = BP;
    BP = SP;
    PC = target;

  } else if(op == call2) {
    w target = pop();
    SP++;
    SP++;
    push(PC);
    push(AP);
    push(BP);
    AP = BP;
    BP = SP;
    PC = target;

  } else if(op == call2_w) {
    w target = memRead(PC);
    PC++;
    SP++;
    SP++;
    push(PC);
    push(AP);
    push(BP);
    AP = BP;
    BP = SP;
    PC = target;

  } else if(op == ret) {
    SP = BP;
    BP = pop();
    AP = pop();
    PC = pop();
  } else if(op == jump_w) {
      w target = memRead(PC);
      PC = target;
  } else if(op == jump) {
      w target = pop();
      PC = target;
  } else if(op == discard_b) {
      SP -= IRHigh();
  } else if(op == discard_w) {
      w discardAmount = memRead(PC);
      PC++;
      SP -= discardAmount;
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
  } else if(op == store2) {
      w valh = pop();
      w vall = pop();
      w target = pop();
      memWrite(target, vall);
      memWrite(target+1, valh);
  } else if(op == rstore) {
      w target = pop();
      w val = pop();
      memWrite(target, val);
  } else if(op == rstore2) {
      w target = pop();
      w valh = pop();
      w vall = pop();
      memWrite(target, vall);
      memWrite(target + 1, valh);
  } else if(op == dup_op) {
      RA = pop();
      push(RA);
      push(RA);

  } else if(op == ei) {
    this->intEnabled = 1;

  } else if(op == di) {
    this->intEnabled = 0;

  } else if(op == pushap) {
    push(AP);

  } else if(op == popap) {
    AP = pop();

  } else if(op == pushbp) {
    push(BP);

  } else if(op == popbp) {
    BP = pop();

  } else if(op == loadsp_w) {
    SP = this->memRead(PC);
    PC++;

  } else if(op == iloadsp_w) {
    RA = this->memRead(PC);
    PC++;
    SP = this->memRead(RA);

  } else if(op == storesp_w) {
    RA = this->memRead(PC);
    PC++;
    this->memWrite(RA, SP);
  } else if(op == syscall_op) {
    intCtl->request(0);

  } else if(op == reti) {
    AP = pop();
    BP = pop();
    PC = pop();
    SP = pop();

  } else {
      printf("op not implemented! %d\n", op);
      printf("op not implemented! %s\n", oplist[op]);
      exit(1);
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
    for(int i = 0; i<=0xffff; i+=2) {
        writeSeq(((w)((uint8_t)image[i]) << 8) | (uint8_t)image[i + 1] );
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

void onSignal(int signal) {
  if(signal == SIGINT) {
    myCpu.terminate();

    tcsetattr(STDIN_FILENO,TCSANOW,&old_tio);
    fcntl(STDIN_FILENO, F_SETFL, oldf);
    exit(1);
  }
}

int main(int argc, char ** argv) {

    signal(SIGINT, onSignal);
    if(argc < 2)
        return 0;

    if(!strcmp(argv[1], "-test")) {
        test();
    } else {
        int debug = 0;
        int ips = 0;
        if(argc > 2) {
            if(!strcmp(argv[2], "-d")) {
                //debug = 1;
                ips = atol(argv[3]);
            }
        }

        printf("Loading binary %s...\n", argv[1]);
        myCpu.setDebug(debug);
        myCpu.setPC(0);
        myCpu.loadBin(0,std::string(argv[1]));
        printf("\nLoading done. Starting..\n");

        unsigned char c;
        /* get the terminal settings for stdin */
        tcgetattr(STDIN_FILENO,&old_tio);

        /* we want to keep the old setting to restore them a the end */
        new_tio=old_tio;

        /* disable canonical mode (buffered i/o) and local echo */
        new_tio.c_lflag &=(~ICANON & ~ECHO);

        /* set the new settings immediately */
        tcsetattr(STDIN_FILENO,TCSANOW,&new_tio);

        oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
//        fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

/*
        int cnt = 0;
        long long prevTime = gettime_ms();
        while(1) {
            cnt++;
            if(cnt > 1000) {

                prevTime = cTime;
                cnt = 0;
            }
            myCpu.execute();
            //if(debug)
            //    usleep(500*1000);
        }
        return 0;
*/

        SP_min = myCpu.getSP();
        SP_max = myCpu.getSP();

        long long nsPerInstr = 0;
        if(ips) {
          nsPerInstr = 1000000000LL / ips;
        }

        printf("IPS: %d = %d ns/instr\n", ips, nsPerInstr);

        startTime = gettime_ms();
        totalInstr = 0;
        while(1) {
            struct timespec tsStart;
            if(ips) {

#ifdef __MACH__ // OS X does not have clock_gettime, use clock_get_time
                clock_serv_t cclock;
                mach_timespec_t mts;
                host_get_clock_service(mach_host_self(), CALENDAR_CLOCK, &cclock);
                clock_get_time(cclock, &mts);
                mach_port_deallocate(mach_task_self(), cclock);
                tsStart.tv_sec = mts.tv_sec;
                tsStart.tv_nsec = mts.tv_nsec;
#else
                clock_gettime(CLOCK_REALTIME, &tsStart);
#endif
            }
            myCpu.execute();

            totalInstr++;

            if(myCpu.getSP() > SP_max) {
                SP_max = myCpu.getSP();
            }

            if(myCpu.getSP() < SP_min) {
                SP_min = myCpu.getSP();
            }
            if(ips) {
              while(1) {
                struct timespec tsEnd;
                struct timespec tsDelay;
                long long nsSpent = 0;
                long long nsDelay = 0;

#ifdef __MACH__ // OS X does not have clock_gettime, use clock_get_time
                clock_serv_t cclock;
                mach_timespec_t mts;
                host_get_clock_service(mach_host_self(), CALENDAR_CLOCK, &cclock);
                clock_get_time(cclock, &mts);
                mach_port_deallocate(mach_task_self(), cclock);
                tsEnd.tv_sec = mts.tv_sec;
                tsEnd.tv_nsec = mts.tv_nsec;
#else
                clock_gettime(CLOCK_REALTIME, &tsEnd);
#endif
                nsSpent = (tsEnd.tv_sec * 1000000000LL + tsEnd.tv_nsec) - (tsStart.tv_sec*1000000000LL + tsStart.tv_nsec);

                nsDelay = nsPerInstr - nsSpent;
                if(nsDelay <= 0) {
                  break;
                }

              }
            }

        }

    }
    return 0;
}
