#include "stdio.h"
#include "sys/types.h"
#include "malloc.h"

struct Marker {
    struct Marker *next;
    struct Marker *prev;
    size_t size;
    int isFree;
};

typedef struct Marker Marker_t;

Marker_t *malloc_head;

void malloc_init(size_t begin, size_t end) {
    //  printf("Malloc init 0x%04x - 0x%04x\n", begin, end);

    malloc_head = (Marker_t *)begin;
    malloc_head->next = 0;
    malloc_head->prev = 0;
    malloc_head->size = (end - begin);
    malloc_head->isFree = 1;
}

extern int __data_end;
void malloc_init_auto() {
  malloc_init((size_t)(&__data_end)+0x2000, 0xFF00);
//  malloc_init(0x8000, 0xFF00);
}

void dumpList() {
    Marker_t *iter = malloc_head;
    while (iter) {
        printf("iter: 0x%04x : prev 0x%04x next 0x%04x size 0x%04x free %d\n",
               iter, iter->prev, iter->next, iter->size, iter->isFree);
        iter = iter->next;
    }
}

void malloc_sanity_check() {
    Marker_t *iter = malloc_head;
    while (iter) {
		if(!iter->size) {
			printf("iter zero size!: 0x%04x : prev 0x%04x next 0x%04x size 0x%04x free %d\n",
				   iter, iter->prev, iter->next, iter->size, iter->isFree);
		}
		iter = iter->next;
    }
}

static void some_test_func(int a, int b) { return; }
//static void some_test_func2() { return; }

void *malloc(size_t sz) {
    Marker_t *iter = malloc_head;
	size_t pSize;
	//malloc_sanity_check();
    //printf("malloc call\n");
    //printf("Before\n");
    //dumpList();
    while (iter) {
        if (iter->isFree) {
            if (iter->size > sz + 2 * sizeof(Marker_t)) {
                Marker_t *newEntry;
                newEntry = (Marker_t *)((size_t)iter + sz + sizeof(Marker_t));
//                printf("iter 0x%04X newEntry 0x%04X sz %d iter sz %d\n", iter, newEntry, sz, iter->size);
//                printf("malloc page copy begin\n");
//                dumpList();
				//printf("Fault here?...\n");
                newEntry->prev = iter;
				//printf("End fault\n");
//                printf("malloc page copy end\n");
//                dumpList();
//                printf("iter 0x%04X newEntry 0x%04X sz %d iter sz %d\n", iter, newEntry, sz, iter->size);
                newEntry->next = iter->next;
                if (newEntry->next) {
                    newEntry->next->prev = newEntry;
                }
				pSize = iter->size;
				//printf("pSize %d iter->size %d\n", pSize, iter->size);
                newEntry->size = iter->size - sz - (unsigned int)sizeof(Marker_t);
                //asm("blink_w 0x0010");
                //some_test_func(0,0);
                //asm("blink_w 0x0020");
                //printf("blah\n");
                newEntry->isFree = 1;
                iter->next = newEntry;
                iter->size = sz + (unsigned int)sizeof(Marker_t);
                iter->isFree = 0;
                //printf("new entry size 0x%04X iter size 0x%04X sz 0x%04x\n", newEntry->size, iter->size, sz);
                //        printf("After 0\n");
                //        dumpList();
                //        printf("Malloc : Ret iter 0x%04x\n",iter);
                return (void *)((size_t)iter + (size_t)sizeof(Marker_t));
            } else if (iter->size >= sz + sizeof(Marker_t)) {
                //iter->isFree = 0;
                //        printf("After 1\n");
                //        dumpList();
                //        printf("Malloc : Ret iter 0x%04x\n",iter);
                return (void *)((size_t)iter + (size_t)sizeof(Marker_t));
            } else {
                //printf("After 2\n");
                //dumpList();
                return 0;

            }
        }

        iter = iter->next;
    }
    //asm("blink_w 0x3456");
    return 0; /* hit end of list */
}

void free(void *ptr) {
    Marker_t *it;
	//malloc_sanity_check();
      //printf("Before free\n");
      //dumpList();

    it = (Marker_t *)((size_t)ptr - (size_t)sizeof(Marker_t));
    it->isFree = 1;
//      printf("Free iter 0x%04x\n",it);
    if (it->next) {
        if (it->next->isFree) {
//                  printf("Merge forward\n");
            it->size = it->size + it->next->size;
            it->next = it->next->next;
            if (it->next) {
                it->next->prev = it;
            }
        }
    }

    if (it->prev) {
        if (it->prev->isFree) {
//                  printf("merge backwards\n");
            it->prev->size = it->prev->size + it->size;
            it->prev->next = it->next;
            if (it->next) {
                it->next->prev = it->prev;
            }
        }
    }
      //printf("After free\n");
      //dumpList();
}

void *realloc(void *ptr, size_t size) {
  void * newPtr;
  struct Marker * header;
  size_t copySize;

  if (size == 0) {
      free(ptr);
      return NULL;
  }

  if (ptr == NULL)
      return malloc(size);


  header = (struct Marker *)((size_t)ptr - (size_t)sizeof(struct Marker));
  size = header->size;
  newPtr = malloc(size);

  copySize = (header->size > size) ? size : header->size;

  memcpy(newPtr, ptr, copySize);
  free(ptr);
  return newPtr;
}

