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


std::map<int, int> freqs;

unsigned int blinkCnt = 0;


long long gettime_ms() {
    struct timeval te;
    gettimeofday(&te, NULL);
    long long milliseconds = te.tv_sec*1000LL + te.tv_usec/1000;
    return milliseconds;
}


enum {
    C_SEG_CODE = 0,
    C_SEG_DATA,
    C_SEG_IO
};


Cpu myCpu;
struct termios old_tio, new_tio;
int oldf;
bool stop = false;

VMemDevice::VMemDevice(Cpu * _cpu) : cpu(_cpu) {
}

void VMemDevice::regInCPU(std::function<bool(size_t)> selectFn, std::function<size_t(size_t)> transformFn) {
    myCpu.regDevice(this, selectFn, transformFn);
}
void VMemDevice::regMMUTableInCPU() {
    myCpu.demux->regMMUTable(reinterpret_cast<MMUTable *> (this));
}
void VMemDevice::regRamInCPU(std::function<bool(size_t)> selectFn, std::function<size_t(size_t)> transformFn) {
    myCpu.regRam(this, selectFn, transformFn);
}


void Demux::memWrite(size_t effAddr, w val, w mmuSelector, w mmuEnabled) {
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
    size_t reqAddr = effAddr; //get real address from mmu process selector->pagetable
    //write realaddress to ram here
    size_t flags = 0;
    if(!mmuEnabled) {
      effAddr &= 0xffff;
    } else {
		//if((effAddr & 0xf000) == (13 << 12)) printf("Write to page 13!\n");
		
      flags = mmuTable->getFlags(effAddr, mmuSelector);
      effAddr = mmuTable->getRealAddress(effAddr, mmuSelector);
      //get real address
      if((flags & PAGE_FLAG_NOT_PRESENT) || (flags & PAGE_FLAG_READ_ONLY)) {
        cpu->MMULastFailPage = mmuTable->getRealPage(reqAddr, mmuSelector);
        //printf("Set faulty page to %d : page %d sel %d code %d\n", cpu->MMULastFailPage, cpu->MMULastFailPage&0xff, (cpu->MMULastFailPage >> 8)&0xf, (cpu->MMULastFailPage & (1<<12)));
        intCtl->request(INT_MMU_LINE);


        printf("Page fault on write with instruction %d %s\n",cpu->IR & 0xff, oplist[cpu->IR & 0xff]);
        throw 1;
      }

    }
    for(const auto & ram : rams) {
      if(ram(effAddr)) {
        return ram.dev->write(ram.transformFunc(effAddr), val, 0, 0);
      }
    }
  }
  printf("WRITE: Device for address 0x%08X not found!\n", effAddr);
  throw 0;
}
w Demux::memRead(size_t effAddr, w mmuSelector, w mmuEnabled) {
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
    size_t reqAddr = effAddr; //get real address from mmu process selector->pagetable
    size_t flags = 0;
    if(!mmuEnabled) {
      effAddr &= 0xffff;
    } else {
		//if((effAddr & 0xf000) == (13 << 12)) printf("Read from page 13!\n");

      flags = mmuTable->getFlags(effAddr, mmuSelector);
      //get real address
      effAddr = mmuTable->getRealAddress(effAddr, mmuSelector);
      if((flags & PAGE_FLAG_NOT_PRESENT)) {
        //printf("Page fault on read\n");
        cpu->MMULastFailPage = mmuTable->getRealPage(reqAddr, mmuSelector);
        //printf("Set faulty page to %d : page %d sel %d code %d\n", cpu->MMULastFailPage, cpu->MMULastFailPage&0xff, (cpu->MMULastFailPage >> 8)&0xf, (cpu->MMULastFailPage & (1<<12)));

        intCtl->request(INT_MMU_LINE);
        throw 1;
      }

    }
    //write realaddress to ram here
    for(const auto & ram : rams) {
      if(ram(effAddr)) {
        return ram.dev->read(ram.transformFunc(effAddr), 0);
      }
    }
  }
  printf("READ: Device for address 0x%08X not found!\n", effAddr);
  throw 0;
  usleep(100*1000);
  return 0;
}



Cpu::Cpu() {

    flDebug = 0;

    intEnabled = 0;
    userMode = 0;

    MMUEnabled = 0;

    MMUEntrySelector = 0;


    this->demux = new Demux();
    this->demux->cpu = this;



    intCtl = new InterruptController(8);


    this->devices.push_back(new RAM(0, 1024*1024)); //ROM


	std::vector<std::istream *> uartSources;
	std::vector<std::ostream *> uartTargets;
	std::vector<int> uartInts;
	uartSources.push_back(&std::cin);

	uartTargets.push_back(NULL);
	
	uartInts.push_back(INT_UART_LINE);
	
    this->devices.push_back(new UART(intCtl,uartInts,std::vector<int>(1), uartSources, uartTargets));
    this->devices.push_back(new PORT(0, NULL, &std::cout));

    this->devices.push_back(new LCD(320, 240, intCtl));
    this->devices.push_back(new HDD(std::string("hda"),0));
    this->devices.push_back(new HDD(std::string("hdb"),1));
    this->devices.push_back(new Timer(intCtl, 3, 5000ULL));
    this->devices.push_back(new MMUTable());
    this->devices.push_back(intCtl);

    this->demux->intCtl = intCtl;

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

    dumpRegs();

    for(int i = 0; i<this->devices.size(); i++) {
        this->devices[i]->terminate();
    }

    printf("SP_min: 0x%04x\nSP_max 0x%04x\n", SP_min, SP_max);
    printf("Average ips %d\n", ipsActual);

    std::map<int,int>::iterator it = freqs.begin();
    while(it != freqs.end()) {
        printf("%s : (%f%%) %d\n", oplist[it->first], static_cast<float>(it->second) / static_cast<float>(totalInstr) * 100.0f, it->second);
        ++it;
    }

    printf("Blinked %d times\n", blinkCnt);
}
void Cpu::memWrite(size_t addr, w val, int seg) {
    //construct effective address
    //       20    19 18 17 16 15 14 ... 1 0
    //data:  priv  0  0  0  0  __data_addr__
    //code:  priv  1  ______code_addr_______
    size_t effAddr = 0;
    if(seg == C_SEG_DATA) {
        effAddr |= (addr&0xffff);
    } else if(seg == C_SEG_CODE) {
        effAddr |= ((addr&0x7ffff) | (1 << 19));
    } else if(seg == C_SEG_IO) {
        effAddr |= ((addr&0x7ffff) | (1 << 20));

    } else {
        printf("Try access unknown .seg %d\nexiting\n", seg);
    }

    //    if(userMode) {
    //        effAddr |= (1<<20);
    //    }

    this->demux->memWrite(effAddr, val, MMUEntrySelector, MMUEnabled);

    /*
    for(int i = 0; i<this->devices.size(); i++) {
      if(this->devices[i]->canOperate(addr)) {
        this->devices[i]->write(addr,val,seg);
        if(0&&flDebug) printf("memWrite [0x%04x] : 0x%04x\n", addr, val);
      }
    }
*/
}

w Cpu::memRead(size_t addr, int seg) { 

    //construct effective address
    //       20    19 18 17 16 15 14 ... 1 0
    //data:   0    0  0  0  0  __data_addr__
    //code:   0    1  ______code_addr_______
    //io  :   1    0  ________io_addr_______

    size_t effAddr = 0;
    if(seg == C_SEG_DATA) {
        effAddr |= (addr&0xffff);
    } else if(seg == C_SEG_CODE) {
        effAddr |= ((addr&0x7ffff) | (1 << 19));
    } else if(seg == C_SEG_IO) {
        effAddr |= ((addr&0x7ffff) | (1 << 20));

    } else {
        printf("Try access unknown .seg %d\nexiting\n", seg);
    }
    //    if(userMode) {
    //        effAddr |= (1<<20);
    //    }

    return this->demux->memRead(effAddr, MMUEntrySelector, MMUEnabled);
}

void Cpu::tick() {


    //insert delay here
    //1000 kHz
    //usleep(1);
    //

}

void Cpu::push(w val) {
    memWrite(SP, val, C_SEG_DATA);
    SP++;
}

w Cpu::pop() {
    SP--;
    w res = memRead(SP, C_SEG_DATA);
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
/*
	static int __pre_int_en = intEnabled;
	if(__pre_int_en != intEnabled) {
		__pre_int_en = intEnabled;
		printf("IntEnable switch! %d\n", intEnabled);
	}
*/
    if(intEnabled) {
        //    printf("CPU: IRQ Line Status: %d\n", intCtl->irqLineStatus());
        if(intCtl->irqLineStatus()) {
            //printf("Into int vec!\n");
/*
            this->push(highPC());
            this->push(BP);
            this->push(AP);
            this->push(S);
            this->push(D);
            this->push(SW());
*/

          //printf("INT PC %08X SP %04X\n", PC, SP);
            this->userMode = 0;
            this->intEnabled = 0;
            this->MMUEntrySelector = 0;
            this->commitEnabled = 0;
            //printf("Try get vec..");
            set_highPC(intCtl->getIrqVector());
            reset_mPC();
            //printf(" 0x%04x\n", PC);
        }
    }


    this->IR = this->memRead(PC, C_SEG_CODE);
    if(flDebug /* || (IR&0xff) == ret2|| (IR&0xff) == le_w|| (IR&0xff) == lt_w */) {
        printf("PC: 0x%08X, SP: 0x%04X, IR: 0x%04X ('%s') ARG: 0x%04X\n", PC, SP, IR, oplist[IR&0xff],this->memRead(PC+1, C_SEG_CODE));
        dumpRegs();
        /*
      printf("PC: 0x%04X, IR: 0x%04X ('%s')\n", PC, IR, oplist[IR&0xff]);
      printf("Stack: ");
      for(int i = 0; i<10; i++) {
          w val =  memRead(SP + i - 10, C_SEG_DATA);
         printf("0x%04X(%c) ", val, (val > 32 && val < 127)?(char)val:'-');
      }
      printf("\n");
*/
    }


    this->PC++;

    unsigned char op = this->IR & 0xff;

    freqs[(int)op]++;

    if(op == nop){
        //this is nop, come on
    } else if(op == addrf_b) {
        this->push(this->AP + IRHigh());
    } else if(op == addrf_w) {
        w tmp = this->memRead(PC, C_SEG_CODE);
        this->PC++;
        this->push(this->AP + tmp);
    } else if(op == iaddrf_b) {
        this->push(memRead(this->AP + IRHigh(), C_SEG_DATA));
    } else if(op == iaddrf_w) {
        w tmp = this->memRead(PC, C_SEG_CODE);
        this->PC++;
        this->push(memRead(this->AP + tmp, C_SEG_DATA));


    } else if(op == addrl_b) {
        this->push(this->BP + IRHigh());
    } else if(op == addrl_w) {
        w tmp = this->memRead(PC,C_SEG_CODE);
        this->PC++;
        this->push(this->BP + tmp);
    } else if(op == iaddrl_b) {
        this->push(memRead(this->BP + IRHigh(), C_SEG_DATA));
    } else if(op == iaddrl_w) {
        w tmp = this->memRead(PC, C_SEG_CODE);
        this->PC++;
        this->push(memRead(this->BP + tmp, C_SEG_DATA));


    } else if(op == cnst_b) {
        this->push(IRHigh());
    } else if(op == cnst_w) {
        w tmp = this->memRead(PC, C_SEG_CODE);
        this->PC++;
        this->push(tmp);
    } else if(op == icnst_b) {
        this->push(memRead(IRHigh(), C_SEG_DATA));
    } else if(op == icnst_w) {
        w tmp = this->memRead(PC, C_SEG_CODE);
        this->PC++;
        this->push(memRead(tmp, C_SEG_DATA));


    } else if(op == addrs_b) {
        this->push(this->SP + IRHigh());
    } else if(op == addrs_w) {
        w tmp = this->memRead(PC, C_SEG_CODE);
        this->PC++;
        this->push(this->SP + tmp);
    } else if(op == iaddrs_b) {
        this->push(memRead(this->SP + IRHigh(), C_SEG_DATA));
    } else if(op == iaddrs_w) {
        w tmp = this->memRead(PC, C_SEG_CODE);
        this->PC++;
        this->push(memRead(this->SP + tmp, C_SEG_DATA));


    } else if(op == indir) {
        this->push(memRead(this->pop(), C_SEG_DATA));

    } else if(op == indir2) {


        w tmp = this->pop();
        this->push(memRead(tmp, C_SEG_DATA));
        this->push(memRead(tmp+1, C_SEG_DATA));


    } else if(op == add) {
        push(pop()+pop());
    } else if(op == add_b) {
        push(pop()+IRHigh());
    } else if(op == add_w) {
        w tmp = this->memRead(PC, C_SEG_CODE);
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
        w tmp = this->memRead(PC, C_SEG_CODE);
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
        w tmp = this->memRead(PC, C_SEG_CODE);
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
        w tmp = this->memRead(PC, C_SEG_CODE);
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
        w tmp = this->memRead(PC, C_SEG_CODE);
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
        w addr = this->memRead(PC, C_SEG_CODE);
        PC++;
        if(v1 == v2) {
            set_highPC(addr);
            reset_mPC();
        }

    } else if(op == ge_w) {
        int v2 = (ws)pop();
        int v1 = (ws)pop();
        w addr = this->memRead(PC, C_SEG_CODE);
        PC++;
        if(v1 >= v2) {
            set_highPC(addr);
            reset_mPC();
        }

    } else if(op == gt_w) {
        int v2 = (ws)pop();
        int v1 = (ws)pop();
        w addr = this->memRead(PC, C_SEG_CODE);
        PC++;
        if(v1 > v2) {
            set_highPC(addr);
            reset_mPC();
        }

    } else if(op == le_w) {
        int v2 = (ws)pop();
        int v1 = (ws)pop();
        w addr = this->memRead(PC, C_SEG_CODE);
        PC++;
        if(v1 <= v2) {
            set_highPC(addr);
            reset_mPC();
        }

    } else if(op == lt_w) {
        int v2 = (ws)pop();
        int v1 = (ws)pop();
        w addr = this->memRead(PC, C_SEG_CODE);
        PC++;
        if(v1 < v2) {
            set_highPC(addr);
            reset_mPC();
        }

    } else if(op == ne_w) {
        int v2 = (ws)pop();
        int v1 = (ws)pop();
        w addr = this->memRead(PC, C_SEG_CODE);
        PC++;
        if(v1 != v2) {
            set_highPC(addr);
            reset_mPC();
        }

    } else if(op == uge_w) {
        w v2 = pop();
        w v1 = pop();
        w addr = this->memRead(PC, C_SEG_CODE);
        PC++;
        if(v1 >= v2) {
            set_highPC(addr);
            reset_mPC();
        }

    } else if(op == ugt_w) {
        w v2 = pop();
        w v1 = pop();
        w addr = this->memRead(PC, C_SEG_CODE);
        PC++;
        if(v1 > v2) {
            set_highPC(addr);
            reset_mPC();
        }

    } else if(op == ule_w) {
        w v2 = pop();
        w v1 = pop();
        w addr = this->memRead(PC, C_SEG_CODE);
        PC++;
        if(v1 <= v2) {
            set_highPC(addr);
            reset_mPC();
        }

    } else if(op == ult_w) {
        w v2 = pop();
        w v1 = pop();
        w addr = this->memRead(PC, C_SEG_CODE);
        PC++;
        if(v1 < v2) {
            set_highPC(addr);
            reset_mPC();
        }

    } else if(op == call0) {
        w target = pop();
        push(highPC());
        push(mPC());
        push(AP);
        push(BP);
        AP = BP;
        BP = SP;
        set_highPC(target);
        reset_mPC();

    } else if(op == call0_w) {
        w target = memRead(PC, C_SEG_CODE);
        PC++;
        push(highPC());
        push(mPC());
        push(AP);
        push(BP);
        AP = BP;
        BP = SP;
        set_highPC(target);
        reset_mPC();

    } else if(op == call1) {
        w target = pop();
        SP++;
        push(highPC());
        push(mPC());
        push(AP);
        push(BP);
        AP = BP;
        BP = SP;
        set_highPC(target);
        reset_mPC();

    } else if(op == call1_w) {
        w target = memRead(PC, C_SEG_CODE);
        PC++;
        SP++;
        push(highPC());
        push(mPC());
        push(AP);
        push(BP);
        AP = BP;
        BP = SP;
        set_highPC(target);
        reset_mPC();

    } else if(op == call2) {
        w target = pop();
        SP++;
        SP++;
        push(highPC());
        push(mPC());
        push(AP);
        push(BP);
        AP = BP;
        BP = SP;
        set_highPC(target);
        reset_mPC();

    } else if(op == call2_w) {
        w target = memRead(PC, C_SEG_CODE);
        PC++;
        SP++;
        SP++;
        push(highPC());
        push(mPC());
        push(AP);
        push(BP);
        AP = BP;
        BP = SP;
        set_highPC(target);
        reset_mPC();

    } else if(op == ret) {
        SP = BP;
        BP = pop();
        AP = pop();
        set_mPC(pop());
        set_highPC(pop());
    } else if(op == jump_w) {
        w target = memRead(PC, C_SEG_CODE);
        set_highPC(target);
        reset_mPC();
    } else if(op == jump) {
        w target = pop();
        set_highPC(target);
        reset_mPC();
    } else if(op == discard_b) {
        SP -= IRHigh();
    } else if(op == discard_w) {
        w discardAmount = memRead(PC, C_SEG_CODE);
        PC++;
        SP -= discardAmount;
    } else if(op == alloc_b) {
        SP += IRHigh();
    } else if(op == alloc_w) {
        w allocAmount = memRead(PC, C_SEG_CODE);
        PC++;
        SP += allocAmount;
    } else if(op == store) {
        w val = pop();
        w target = pop();
        memWrite(target, val, C_SEG_DATA);
    } else if(op == store2) {
        w valh = pop();
        w vall = pop();
        w target = pop();
        memWrite(target, vall, C_SEG_DATA);
        memWrite(target+1, valh, C_SEG_DATA);
    } else if(op == rstore) {
        w target = pop();
        w val = pop();
        memWrite(target, val, C_SEG_DATA);
    } else if(op == rstore2) {
        w target = pop();
        w valh = pop();
        w vall = pop();
        memWrite(target, vall, C_SEG_DATA);
        memWrite(target + 1, valh, C_SEG_DATA);
    } else if(op == dup_op) {
        RA = pop();
        push(RA);
        push(RA);

    } else if(op == ei) {
        this->intEnabled = 1;

    } else if(op == di) {
        this->intEnabled = 0;

    } else if(op == ec) {
        this->commitEnabled = 1;

    } else if(op == dc) {
        this->commitEnabled = 0;

    } else if(op == pushc) {
        pushCommit();
    } else if(op == pushmmu) {
        push(MMULastFailPage);

    } else if(op == pushap) {
        push(AP);

    } else if(op == popap) {
        AP = pop();

    } else if(op == pushbp) {
        push(BP);

    } else if(op == popbp) {
        BP = pop();

    } else if(op == loadsp_w) {
        SP = this->memRead(PC, C_SEG_CODE);
        PC++;

    } else if(op == iloadsp_w) {
        RA = this->memRead(PC, C_SEG_CODE);
        PC++;
        SP = this->memRead(RA, C_SEG_DATA);

    } else if(op == storesp_w) {
        RA = this->memRead(PC, C_SEG_CODE);
        PC++;
        this->memWrite(RA, SP, C_SEG_DATA);

	//printf("Store SP: 0x%04X -> 0x%04X\n", SP, RA);

    } else if(op == syscall_op) {
        intCtl->request(0);

    } else if(op == reti) {
        D = pop();
        S = pop();
        AP = pop();
        BP = pop();
        set_highPC(pop());
        RA = pop(); //SP
        w sw = pop();
        set_SW(sw);
        //printf("RETI : restore sw 0x%08X\n", sw);
        SP = RA;
        //printf("RETI : restore PC 0x%08X\n", PC);
        //printf("RETI : restore SP 0x%08X\n", SP);
        if(PC == 0) {
          //flDebug = 1;
          //intEnabled = 0;
        }


    } else if(op == swp_b) {
        // sp--
        // sp->ml
        // mr->ra //address
        // ra->ml
        // mr->rb //old value
        // irhigh->mw //write new value
        // sp->ml
        // rb->mw
        // sp++

        RA = pop(); //address
        RB = this->memRead(RA, C_SEG_DATA);
        this->memWrite(RA, IRHigh(), C_SEG_DATA);
        push(RB);


    } else if(op == pops) {
        S = pop();
    } else if(op == loads_w) {
        RA = this->memRead(PC, C_SEG_CODE);
        PC++;
        S = RA;
    } else if(op == pushs) {
        push(S);
    } else if(op == incs) {
        S++;
    } else if(op == decs) {
        S--;


    } else if(op == popd) {
        D = pop();
    } else if(op == loadd_w) {
        RA = this->memRead(PC, C_SEG_CODE);
        PC++;
        D = RA;
    } else if(op == pushd) {
        push(D);
    } else if(op == incd) {
        D++;
    } else if(op == decd) {
        D--;


    } else if(op == sdm) {
        // S -> ml
        // mr -> RA
        // D -> ml
        // RA -> mr

        RA = this->memRead(S, C_SEG_DATA);
        this->memWrite(D, RA, C_SEG_DATA);

    } else if(op == sdmi) {
        // S -> ml
        // mr -> RA
        // D -> ml
        // RA -> mr

        RA = this->memRead(S, C_SEG_DATA);
        this->memWrite(D, RA, C_SEG_DATA);
        S++;
        D++;

        /*
    IO SPACE OPS

    iostore,
    iorstore,
    iostore_b,
    ioread,
    ioread_b

*/

    } else if(op == iostore) {
        w val = pop();
        w target = pop();
        memWrite(target, val, C_SEG_IO);
    } else if(op == iorstore) {
        w target = pop();
        w val = pop();
        memWrite(target, val, C_SEG_IO);
    } else if(op == iostore_b) {
        w target = IRHigh();
        w val = pop();
        memWrite(target, val, C_SEG_IO);
    } else if(op == ioread) {
        this->push(memRead(this->pop(), C_SEG_IO));
    } else if(op == ioread_b) {
        this->push(memRead(IRHigh(), C_SEG_IO));

    } else if(op == mmuon) {
        MMUEnabled = 1;
        printf("Enable MMU!\n");
    } else if(op == mmuoff) {
        MMUEnabled = 0;


    } else if(op == blink_w) {

      RA = this->memRead(PC, C_SEG_CODE);
      PC++;



      printf("---->>> BLINK! 0x%04X\n", RA);
      dumpRegs();
        blinkCnt++;

    } else {
        printf("op not implemented! %d\n", op);
		dumpRegs();
        //printf("op not implemented! %s\n", oplist[op]);
        intCtl->request(INT_DECODE_LINE);
        commitEnabled = false;
    }


    if(commitEnabled) {
      commit();
    }

}


void Cpu::writeSeq(w val) {
    memWrite(seqWriterPos, val, C_SEG_DATA);
    seqWriterPos++;
}

void Cpu::loadBin(w addr, std::string filename) {
    std::ifstream file;
    file.open(filename.c_str(), std::ios::in | std::ios::binary);
    char image[0xffff*2];
    file.read(image, 0xffff*2);
    setSeqWriterPos(addr);
    for(int i = 0; i<0xffff*2; i+=2) {
        writeSeq(((w)((uint8_t)image[i]) << 8) | (uint8_t)image[i + 1] );
    }
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

    int debug = 0;
    int ips = 0;
    if(argc > 2) {
        if(!strcmp(argv[2], "-d")) {
            //debug = 1;
            ips = atol(argv[3]);
        }
        if(!strcmp(argv[2], "-a")) {
            debug = 1;
        }
        if(!strcmp(argv[2], "-t")) {
            myCpu.memWrite(0xA0, 'a' ,C_SEG_IO);
            myCpu.memWrite(0xA0, 'b' ,C_SEG_IO);
            myCpu.memWrite(0xA0, 'c' ,C_SEG_IO);
            myCpu.memWrite(0xA0, 'd' ,C_SEG_IO);
            exit(1);
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

    SP_min = myCpu.getSP();
    SP_max = myCpu.getSP();

    long long nsPerInstr = 0;
    if(ips) {
        nsPerInstr = 1000000000LL / ips;
    }

    printf("IPS: %d = %d ns/instr\n", ips, nsPerInstr);

    startTime = gettime_ms();
    totalInstr = 0;
    std::ofstream log("log");

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

        try {
          myCpu.execute();
        } catch(int i) {
          //printf("SIMULATOR : exception in execute [%d]\n", i);
        }

        //log << gettime_ms() << " " << __current_code_bank << " " << __current_data_bank << " " << myCpu.getPC() << " " << myCpu.getSP() << std::endl;

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

    log.close();
    return 0;
}
