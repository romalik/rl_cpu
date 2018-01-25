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



void mm_init();
unsigned int mm_allocSegment(unsigned int *seg);
void mm_freeSegment(unsigned int seg);
void mm_memcpy(unsigned int *dest, unsigned int *src, unsigned int n,
               unsigned int bank);

#endif
