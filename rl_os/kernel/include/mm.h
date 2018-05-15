#ifndef MM_H__
#define MM_H__
#include <memmap.h>
#include <sched.h>

#define NCODE_PAGES 128
#define NDATA_PAGES 16

#define NBANKS 16

#define FREE_PAGE_MARK 0x1fff

#define MMU_TABLE_FLAGS_MASK 0xe000
#define MMU_TABLE_FLAGS_SHIFT 13
#define MMU_TABLE_NO_FLAGS_MASK 0x1fff

#define PAGE_FLAG_READ_ONLY 1
#define PAGE_FLAG_NOT_PRESENT 2

void mmu_mark_selector(size_t selector, int flag);
size_t mmu_get_free_selector();

void mmu_write_table(size_t process, size_t pageno, size_t s_code, size_t entry);
size_t mmu_read_table(size_t process, size_t pageno, size_t s_code);
void mmu_write_table_flags(size_t process, size_t pageno, size_t s_code, size_t flags);
size_t mmu_read_table_flags(size_t process, size_t pageno, size_t s_code);
void mmu_mark_page(size_t pageno, int flag);

void mmu_inc_page_refcnt(size_t pageno);
void mmu_dec_page_refcnt(size_t pageno);
size_t mmu_get_page_refcnt(size_t pageno);

int check_and_fix_page(struct Process * p, size_t pageno, int is_code, int wr);



size_t mmu_get_free_page();
void mmu_copy_pages(size_t p_src, size_t p_dst, int processToMap, int whereToMapSrc, int whereToMapDst);
void mmu_init();

void mmu_test();

unsigned int getPageAndOffset(struct Process * p, size_t addr, size_t * offset);
unsigned int getPageIdxInProcess(size_t addr);
void freeProcessPages(struct Process * p);

void dumpProcessPages(struct Process * p);


unsigned int ugetc(struct Process * p, size_t addr, size_t processToMap, size_t whereToMap);
void uputc(struct Process * p, size_t addr, size_t processToMap, size_t whereToMap, unsigned int val);
size_t ugets(struct Process * p, size_t addr, size_t processToMap, size_t whereToMap, size_t length, int nullTerminated, unsigned int * s);
size_t uputs(struct Process * p, size_t addr, size_t processToMap, size_t whereToMap, size_t length, int nullTerminated, unsigned int * s);
size_t umemset(struct Process * p, size_t addr, size_t processToMap, size_t whereToMap, size_t length, unsigned int v);


unsigned int null_file_read(unsigned int minor, unsigned int * buf, size_t n);
unsigned int null_file_write(unsigned int minor, const unsigned int * buf, size_t n);

unsigned int zero_file_read(unsigned int minor, unsigned int * buf, size_t n);
unsigned int zero_file_write(unsigned int minor, const unsigned int * buf, size_t n);


#endif
