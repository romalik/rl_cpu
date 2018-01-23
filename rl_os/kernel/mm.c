#include <sched.h>
#include <mm.h>
#include <memmap.h>
#include <kstdio.h>

//pages per proc 256
//processes 64
//width 16 bit
//256*64/16 = 1024
#define pageMapSize 1024
#define pageSize 4096
unsigned int pageMap[pageMapSize];


void mmu_write_table(size_t process, size_t pageno, size_t s_code, size_t entry) {
  size_t target_io_addr = (process << 8) | (s_code << 12) | (pageno & 0xff);

  target_io_addr |= (1<<14); //select mmu from io devs
  outb(target_io_addr, entry);
}

void mmu_mark_page(size_t pageno, int flag) {
  if(flag) {
    pageMap[pageno >> 4] |= (1 << (pageno & 0x0f));
  } else {
    pageMap[pageno >> 4] &= (~(1 << (pageno & 0x0f)));
  }
}

size_t mmu_get_free_page() {
  size_t pageno = 0;
  while(pageMap[pageno >> 4] == 0xffff) {
    pageno += 0x0f;
  }

  while(pageMap[pageno >> 4] & (1 <<(pageno & 0x0f))) { pageno++; }

  return pageno;
}

void mmu_on() {
        asm("mmuon");
        asm("ret");
}

void mmu_copy_pages(size_t p_src, size_t p_dst, int processToMap, int whereToMapSrc, int whereToMapDst) {
  mmu_write_table(processToMap, whereToMapSrc, 0, p_src);
  mmu_write_table(processToMap, whereToMapDst, 0, p_dst);
  memcpy((unsigned int *)(whereToMapDst * pageSize), (unsigned int *)(whereToMapSrc * pageSize), pageSize);
}


void mmu_init() {
  int i;
  for(i = 0; i<pageMapSize; i++) {
    pageMap[i] = 0;
  }
  for(i = 0; i<16; i++) {
    mmu_write_table(0, i, 0, i);
    mmu_write_table(0, i, 1, i);
    mmu_mark_page(i, 1);
  }
  
  mmu_on();
}




unsigned int BankMap[NBANKS];

void mm_init() {
    int i;
    for (i = 0; i < NBANKS; i++) {
        BankMap[i] = 0;
    }
}
void mm_freeSegment(unsigned int seg) {
    BankMap[seg] = 0;
}

unsigned int mm_allocSegment(unsigned int *seg) {
    int i;
    for (i = 0; i < NBANKS; i++) {
        if (BankMap[i] == 0) {
            BankMap[i] = 1;
            *seg = i;
            return 1;
        }
    }
    return 0;
}
void mm_memcpy(unsigned int *dest, unsigned int *src, unsigned int n,
               unsigned int bank) {
    DATA_BANK_SEL = bank;
    memcpy(dest, src, n);
}
