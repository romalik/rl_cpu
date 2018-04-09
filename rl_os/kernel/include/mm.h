#ifndef MM_H__
#define MM_H__
#include <memmap.h>
#include <sched.h>

#define NCODE_PAGES 128
#define NDATA_PAGES 16

#define NBANKS 16

#define FREE_PAGE_MARK 0xffff


void mmu_mark_selector(size_t selector, int flag);
size_t mmu_get_free_selector();

void mmu_write_table(size_t process, size_t pageno, size_t s_code, size_t entry);
size_t mmu_read_table(size_t process, size_t pageno, size_t s_code);
void mmu_mark_page(size_t pageno, int flag);
size_t mmu_get_free_page();
void mmu_copy_pages(size_t p_src, size_t p_dst, int processToMap, int whereToMapSrc, int whereToMapDst);
void mmu_init();

void mmu_test();

unsigned int getPageAndOffset(struct Process * p, size_t addr, size_t * offset);
void freeProcessPages(struct Process * p);

unsigned int ugetc(struct Process * p, size_t addr, size_t processToMap, size_t whereToMap);
void uputc(struct Process * p, size_t addr, size_t processToMap, size_t whereToMap, unsigned int val);
size_t ugets(struct Process * p, size_t addr, size_t processToMap, size_t whereToMap, size_t length, int nullTerminated, unsigned int * s);
size_t uputs(struct Process * p, size_t addr, size_t processToMap, size_t whereToMap, size_t length, int nullTerminated, unsigned int * s);


#endif
