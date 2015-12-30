#include "stdio.h"
#include "types.h"
#include "malloc.h"

struct Marker {
  struct Marker * next;
  struct Marker * prev;
  size_t size;
  int isFree;
};

typedef struct Marker Marker_t;

Marker_t * malloc_head;

void malloc_init(size_t begin, size_t end) {
  printf("Malloc init 0x%04x - 0x%04x\n", begin, end);

  malloc_head = (Marker_t *)begin;
  malloc_head->next = 0;
  malloc_head->prev = 0;
  malloc_head->size = (end - begin);
  malloc_head->isFree = 1;

}


void *malloc(size_t sz) {
  Marker_t * iter = malloc_head;
//  printf("malloc call 0x%04x\n", sz);
  while(iter) {
    printf("iter: 0x%04x : prev 0x%04x next 0x%04x size 0x%04x free %d\n", iter, iter->prev, iter->next, iter->size, iter->isFree);

    if(iter->isFree) {
      if(iter->size > sz + 2*sizeof(Marker_t)) {
        Marker_t * newEntry;
        newEntry = (Marker_t *)((size_t)iter + sz + sizeof(Marker_t));
        newEntry->prev = iter;
        newEntry->next = iter->next;
        newEntry->size = iter->size - sz - sizeof(Marker_t);
        newEntry->isFree = 1;
        iter->next = newEntry;
        iter->size = sz + sizeof(Marker_t);
        iter->isFree = 0;
        return (void *)((size_t)iter + (size_t)sizeof(Marker_t));
      } else if( iter->size >= sz + sizeof(Marker_t)) {
        iter->isFree = 0;
        return (void *)((size_t)iter + (size_t)sizeof(Marker_t));
      }
    }

    iter = iter->next;
  }

  return 0; /* hit end of list */
}

void free(void* ptr) {
  Marker_t * it;
  it = (Marker_t *)((size_t)ptr - (size_t)sizeof(Marker_t));
  it->isFree = 1;
  if(it->next) {
    if(it->next->isFree) {
      it->size = it->size + it->next->size;
      it->next = it->next->next;
        it->next->prev = it;
    }
  }

  if(it->prev) {
    if(it->prev->isFree) {
      it->prev->size = it->prev->size + it->size;
      it->prev->next = it->next;
      if(it->next) {
        it->next->prev = it->prev;
      }
    }
  }
}
