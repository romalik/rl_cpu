#ifndef SET_H__
#define SET_H__

#define DYNAMIC_MEMORY_SUPPORT 0

#include <types.h>
#include <string.h>


#if DYNAMIC_MEMORY_SUPPORT
#include <malloc.h>
#endif


struct set {
  size_t el_size;
  size_t capacity;
  size_t size;
  unsigned int * data;
};

#if DYNAMIC_MEMORY_SUPPORT
struct set * set_create();
#endif

struct set * set_create_static(size_t capacity, size_t el_size, struct set * self, unsigned int * arena);

#if DYNAMIC_MEMORY_SUPPORT
void set_destroy(struct set * cb);
#endif

unsigned int * set_insert(struct set * self, unsigned int * el);

unsigned int * set_at(struct set * self, size_t idx);

void set_remove(struct set * self, size_t idx);
void set_remove_by_element(struct set * self, unsigned int * el);

void set_clear(struct set * self);


#endif
