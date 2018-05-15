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
#define pageMapSize 4096
#define pageSize 4096
unsigned int pageMap[pageMapSize];

unsigned int mmu_stack[2048];

void mmu_interrupt() {
  size_t failedPage = mmu_stack[sizeof(struct InterruptFrame)];
  size_t pageno = failedPage & 0x7f; //discard code flag
  size_t is_code = (failedPage & 0x80) ? 1 : 0;
  check_and_fix_page(cProc, pageno, is_code, 1);
}

void mmu_write_table(size_t process, size_t pageno, size_t s_code, size_t entry) {
  size_t target_io_addr = (process << 8) | (s_code << 12) | (pageno & 0xff);
  target_io_addr |= (1<<14); //select mmu from io devs
  //printf("mmu write page 0x%04X = 0x%04X\n", target_io_addr, entry);
  outb(target_io_addr, entry);
}

void mmu_write_table_flags(size_t process, size_t pageno, size_t s_code, size_t flags) {
  size_t target_io_addr = (process << 8) | (s_code << 12) | (pageno & 0xff);
  size_t entry;
  target_io_addr |= (1<<14); //select mmu from io devs
  //printf("mmu write page 0x%04X = 0x%04X\n", target_io_addr, entry);
  entry = inb(target_io_addr);
  entry &= MMU_TABLE_NO_FLAGS_MASK;
  entry |= (flags << MMU_TABLE_FLAGS_SHIFT);
  outb(target_io_addr, entry);
}

size_t mmu_read_table(size_t process, size_t pageno, size_t s_code) {
  size_t target_io_addr = (process << 8) | (s_code << 12) | (pageno & 0xff);
  size_t entry;
  target_io_addr |= (1<<14); //select mmu from io devs
  entry = inb(target_io_addr) & MMU_TABLE_NO_FLAGS_MASK;
  //printf("mmu read page 0x%04X = 0x%04X\n", target_io_addr, entry);
  return entry;
}

size_t mmu_read_table_flags(size_t process, size_t pageno, size_t s_code) {
  size_t target_io_addr = (process << 8) | (s_code << 12) | (pageno & 0xff);
  size_t entry;
  target_io_addr |= (1<<14); //select mmu from io devs
  entry = ((inb(target_io_addr) & MMU_TABLE_FLAGS_MASK)>>MMU_TABLE_FLAGS_SHIFT) ;
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
  pageMap[pageno] = flag;
}

void mmu_inc_page_refcnt(size_t pageno) {
  pageMap[pageno]++;
}

void mmu_dec_page_refcnt(size_t pageno) {
  pageMap[pageno]--;
}

size_t mmu_get_page_refcnt(size_t pageno) {
  return pageMap[pageno];
}


size_t mmu_get_free_page() {
  size_t pageno = 0;
  while(pageMap[pageno]) { pageno++; }

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


int check_and_fix_page(struct Process * p, size_t pageno, int is_code, int wr) {
  int result = 0;
  size_t flags;
  flags = mmu_read_table_flags(p->mmuSelector, pageno, is_code);

  if(flags != 0) {
    //action required
//      printf("pid %d\n", p->pid);
//        printf("before mmu copy:\n");
//      dumpProcessPages(p);
    if((flags & PAGE_FLAG_READ_ONLY) && wr) {
      //want to write to write-protected page
      size_t src_page;
      size_t target_page;
      size_t refcnt;
      //printf("check_and_fix_page: flags = %d mmuSelector %d pageno %d is_code %d\n", flags, p->mmuSelector, pageno, is_code);
      src_page = mmu_read_table(p->mmuSelector, pageno, is_code);
      refcnt = mmu_get_page_refcnt(src_page);
      if(refcnt > 1) {
        mmu_dec_page_refcnt(src_page);

        target_page = mmu_get_free_page();
        mmu_mark_page(target_page, 1);
        mmu_write_table(p->mmuSelector, pageno, is_code, target_page);
        mmu_copy_pages(src_page, target_page, 0, 14, 15);
        //printf("copy page %04d -> %04d\n", src_page, target_page);
        //printf("copy page\n");
        result = 1;
      } else {
        target_page = src_page;
        //printf("no copy page %04d\n", src_page);
      }
      mmu_write_table_flags(p->mmuSelector, pageno, is_code, 0);

    }
    //printf("after mmu copy pid %d:\n", p->pid);
    //dumpProcessPages(p);
  }
  return result;

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
    size_t pageno = 0;
    pageno = mmu_read_table(p->mmuSelector, (addr >> 12), 0);
    if(offset) *offset = addr & 0xfff;
    return pageno;
}

unsigned int getPageIdxInProcess(size_t addr) {
  return addr >> 12;
}

unsigned int ugetc(struct Process * p, size_t addr, size_t processToMap, size_t whereToMap) {
    size_t offset;
    unsigned int retval = 0;
    if(check_and_fix_page(p, getPageIdxInProcess(addr), 0, 0)) {
      //printf("ugetc : page relocated\n");
    }
    mmu_write_table(processToMap, whereToMap, 0, getPageAndOffset(p, addr, &offset));
    retval = *(unsigned int *)((whereToMap << 12)|offset);
    return retval;
}

void uputc(struct Process * p, size_t addr, size_t processToMap, size_t whereToMap, unsigned int val) {
    size_t offset;
    if(check_and_fix_page(p, getPageIdxInProcess(addr), 0, 1)) {
      //printf("uputc : page relocated\n");
    }
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
            if(check_and_fix_page(p, getPageIdxInProcess(addr), 0, 0)) {
              //printf("ugets: page relocated\n");
            }
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
            if(check_and_fix_page(p, getPageIdxInProcess(addr), 0, 1)) {
              //printf("uputs: page relocated\n");
            }
            mmu_write_table(processToMap, whereToMap, 0, getPageAndOffset(p, addr, NULL));
        }
        c = (*(unsigned int *)((whereToMap << 12)|(addr & 0xfff)) = *s);
//        printf("uputs : [0x%04X] <- 0x%04X\n", addr, c);
        s++; addr++; length--;
        if(nullTerminated && (!c)) {
            break;
        }
    }
    return addr - begin;
}

size_t umemset(struct Process * p, size_t addr, size_t processToMap, size_t whereToMap, size_t length, unsigned int v) {
    size_t pageno_u = 0xffff; //user's current page region
    unsigned int c = 0;
    size_t begin = addr;
    while(length) {
        if((addr >> 12) != pageno_u) { //page boundary!
            pageno_u = addr >> 12;
            if(check_and_fix_page(p, getPageIdxInProcess(addr), 0, 1)) {
              //printf("uputs: page relocated\n");
            }
            mmu_write_table(processToMap, whereToMap, 0, getPageAndOffset(p, addr, NULL));
        }
        c = (*(unsigned int *)((whereToMap << 12)|(addr & 0xfff)) = v);
//        printf("uputs : [0x%04X] <- 0x%04X\n", addr, c);
        addr++; length--;
    }
    return addr - begin;
}

void dumpProcessPages(struct Process * p) {
  size_t pageno = 0;

  //printf("freeProcessPages : freeing pid %d cProc pid %d\n", p->pid, cProc->pid);
  printf("Pid %d page map\nPage\tReal\tFlags\tRefcnt\tSection\n", p->pid);
  while(pageno < 128) {
      size_t src_page;
      size_t refcnt;
      size_t flags;
      src_page = mmu_read_table(p->mmuSelector, pageno, 1);

      //printf("freeProcessPages : text pageno = %d src_page = 0x%04x\n", pageno, src_page);
      if(src_page == FREE_PAGE_MARK) break;
      //printf("freeProcessPages : text 1\n");
      refcnt = mmu_get_page_refcnt(src_page);
      flags = mmu_read_table_flags(p->mmuSelector, pageno, 1);
      printf("%04d\t%04d\t%02d\t%04d\ttext\n", pageno, src_page, flags, refcnt);
      pageno++;
  }
  pageno = 0;
  while(pageno < 16) {
    size_t src_page;
    size_t refcnt;
    size_t flags;
    src_page = mmu_read_table(p->mmuSelector, pageno, 0);

    //printf("freeProcessPages : text pageno = %d src_page = 0x%04x\n", pageno, src_page);
    if(src_page == FREE_PAGE_MARK) break;
    //printf("freeProcessPages : text 1\n");
    refcnt = mmu_get_page_refcnt(src_page);
    flags = mmu_read_table_flags(p->mmuSelector, pageno, 0);
    printf("%04d\t%04d\t%02d\t%04d\tdata\n", pageno, src_page, flags, refcnt);
    pageno++;
  }
}

void freeProcessPages(struct Process * p) {
    size_t pageno = 0;

    //printf("freeProcessPages : freeing pid %d cProc pid %d\n", p->pid, cProc->pid);
    while(pageno < 128) {
        size_t src_page = mmu_read_table(p->mmuSelector, pageno, 1);
        //printf("freeProcessPages : text pageno = %d src_page = 0x%04x\n", pageno, src_page);
        if(src_page == FREE_PAGE_MARK) break;
        //printf("freeProcessPages : text 1\n");
        mmu_dec_page_refcnt(src_page);
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
        mmu_dec_page_refcnt(src_page);
        mmu_write_table(p->mmuSelector, pageno, 0, FREE_PAGE_MARK);
        pageno++;
    }
}


unsigned int null_file_read(unsigned int minor, unsigned int * buf, size_t n) {
	return 0;
}

unsigned int null_file_write(unsigned int minor, const unsigned int * buf, size_t n) {
    return n;
}

unsigned int zero_file_read(unsigned int minor, unsigned int * buf, size_t n) {
	memset(buf, 0, n);
	return n;
}

unsigned int zero_file_write(unsigned int minor, const unsigned int * buf, size_t n) {
    return n;
}
