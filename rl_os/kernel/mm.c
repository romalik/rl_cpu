#include <sched.h>
#include <mm.h>
#include <memmap.h>
#include <kstdio.h>


#define mmuSelectorNum 64
unsigned int selectorMap[mmuSelectorNum];

//pages per proc 256
//processes 64
//width 16 bit
//256*64/16 = 1024
#define pagesPerProc 256
#define pageNum 16384
#define pageMapSize 1024
#define pageSize 4096
unsigned int pageMap[pageMapSize];

unsigned int pageMap[pageMapSize];


void mmu_write_table(size_t process, size_t pageno, size_t s_code, size_t entry) {
  size_t target_io_addr = (process << 8) | (s_code << 12) | (pageno & 0xff);
  target_io_addr |= (1<<14); //select mmu from io devs
  //printf("mmu write page 0x%04X = 0x%04X\n", target_io_addr, entry);
  outb(target_io_addr, entry);
}

size_t mmu_read_table(size_t process, size_t pageno, size_t s_code) {
  size_t target_io_addr = (process << 8) | (s_code << 12) | (pageno & 0xff);
  size_t entry;
  target_io_addr |= (1<<14); //select mmu from io devs
  entry = inb(target_io_addr);
  //printf("mmu read page 0x%04X = 0x%04X\n", target_io_addr, entry);
  return entry;
}


void mmu_test() {
    size_t i;
    for(i = 0; i<10; i++) {
        mmu_write_table(0, i, 0, i);
        //printf("write to %d val %d\n", i, i);
        //printf("read from %d val %d\n", i, mmu_read_table(0,i,0));
    }

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

  //printf("Get free page : 0x%04x\n", pageno);
  if(pageno < pageNum) return pageno;

  return 0;
}

void mmu_mark_selector(size_t selector, int flag) {
    selectorMap[selector] = flag;
}

size_t mmu_get_free_selector() {
    size_t selector = 0;
    while(selectorMap[selector] != 0) {selector++;}
    if(selector < mmuSelectorNum) return selector;

    return 0;
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
  for(i = 0; i<mmuSelectorNum; i++) {
    selectorMap[i] = 0;
  }
  for(i = 0; i<mmuSelectorNum; i++) {
      int j;
      for(j = 0; j<pagesPerProc/2; j++) {
          mmu_write_table(i, j, 0, FREE_PAGE_MARK);
          mmu_write_table(i, j, 1, FREE_PAGE_MARK);
      }
  }

  for(i = 0; i<16; i++) {
    mmu_write_table(0, i, 0, i);
    mmu_write_table(0, i, 1, i);
    mmu_mark_page(i, 1);
  }
  selectorMap[0] = 1;
  mmu_on();
}

unsigned int getPageAndOffset(struct Process * p, size_t addr, size_t * offset) {
    size_t pageno = mmu_read_table(p->mmuSelector, (addr >> 12), 0);
    if(offset) *offset = addr & 0xfff;
    return pageno;
}

unsigned int ugetc(struct Process * p, size_t addr, size_t processToMap, size_t whereToMap) {
    size_t offset;
    mmu_write_table(processToMap, whereToMap, 0, getPageAndOffset(p, addr, &offset));
    return *(unsigned int *)((whereToMap << 12)|offset);
}

void uputc(struct Process * p, size_t addr, size_t processToMap, size_t whereToMap, unsigned int val) {
    size_t offset;
    mmu_write_table(processToMap, whereToMap, 0, getPageAndOffset(p, addr, &offset));
    *(unsigned int *)((whereToMap << 12)|offset) = val;
}

size_t ugets(struct Process * p, size_t addr, size_t processToMap, size_t whereToMap, size_t length, int nullTerminated, unsigned int * s) {
    size_t pageno_u = 0xffff; //user's current page region
    unsigned int c = 0;
    size_t begin = addr;
    //printf("Try ugets\n");
    while(length) {
        if((addr >> 12) != pageno_u) { //page boundary!
            pageno_u = addr >> 12;
            mmu_write_table(processToMap, whereToMap, 0, getPageAndOffset(p, addr, NULL));
        }
        c = (*s = *(unsigned int *)((whereToMap << 12)|(addr & 0xfff)));
        //printf("ugets [%c] len 0x%04x\n", c, length);
        s++; addr++; length--;
        if(nullTerminated && (!c)) {
            break;
        }
    }
    //printf("Ugets done!\n");

    return addr - begin;
}

size_t uputs(struct Process * p, size_t addr, size_t processToMap, size_t whereToMap, size_t length, int nullTerminated, unsigned int * s) {
    size_t pageno_u = 0xffff; //user's current page region
    unsigned int c = 0;
    size_t begin = addr;
    while(length) {
        if((addr >> 12) != pageno_u) { //page boundary!
            pageno_u = addr >> 12;
            mmu_write_table(processToMap, whereToMap, 0, getPageAndOffset(p, addr, NULL));
        }
        c = (*(unsigned int *)((whereToMap << 12)|(addr & 0xfff)) = *s);
        printf("uputs : [0x%04X] <- 0x%04X\n", addr, c);
        s++; addr++; length--;
        if(nullTerminated && (!c)) {
            break;
        }
    }
    return addr - begin;
}


void freeProcessPages(struct Process * p) {
    size_t pageno = 0;

    //printf("freeProcessPages : freeing pid %d cProc pid %d\n", p->pid, cProc->pid);

    while(pageno < 128) {
        size_t src_page = mmu_read_table(p->mmuSelector, pageno, 1);
        //printf("freeProcessPages : text pageno = %d src_page = 0x%04x\n", pageno, src_page);
        if(src_page == FREE_PAGE_MARK) break;
        //printf("freeProcessPages : text 1\n");
        mmu_mark_page(src_page, 0);
        //printf("freeProcessPages : text 2\n");
        mmu_write_table(p->mmuSelector, pageno, 1, FREE_PAGE_MARK);
        //printf("freeProcessPages : text 3\n");
        pageno++;
    }
    pageno = 0;
    while(pageno < 16) {
        size_t src_page = mmu_read_table(p->mmuSelector, pageno, 0);
        //printf("freeProcessPages : data pageno = %d src_page = 0x%04x\n", pageno, src_page);
        if(src_page == FREE_PAGE_MARK) break;
        mmu_mark_page(src_page, 0);
        mmu_write_table(p->mmuSelector, pageno, 0, FREE_PAGE_MARK);
        pageno++;
    }

}

