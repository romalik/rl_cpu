#include <set.h>
#if DYNAMIC_MEMORY_SUPPORT
struct set * set_create();
#endif

struct set * set_create_static(size_t capacity, size_t el_size, struct set * self, unsigned int * arena) {
  self->capacity = capacity;
  self->el_size = el_size;
  self->data = arena;
  set_clear(self);
  return self;
}

#if DYNAMIC_MEMORY_SUPPORT
void set_destroy(struct set * cb);
#endif

unsigned int * set_insert(struct set * self, unsigned int * el) {
  size_t i = 0;
  if(self->size >= self->capacity) {
#if DYNAMIC_MEMORY_SUPPORT
    //allocate additional space
#else
  panic("set_insert: overflow");
#endif
  }
  for(i = 0; i<self->capacity; i++) {
    if(!self->data[i*(self->el_size+1)]) {
      self->size++;
      self->data[i*(self->el_size+1)] = 1;
      memcpy(self->data + i*(self->el_size+1) + 1, el, self->el_size);
      return self->data + i*(self->el_size+1) + 1;
    }
  }
  panic("set_insert: corrupt");
  return 0;
}

unsigned int * set_at(struct set * self, size_t idx) {
  size_t i = 0;
  size_t ii = 0;
  if(idx >= self->size) {
    panic("set_at: out of bounds");
  }
  for(i = 0; i<self->capacity; i++) {
    if(self->data[i*(self->el_size+1)]) {
      if(ii == idx) {
        return self->data + i*(self->el_size+1) + 1;
      } else {
        ii++;
      }
    }
  }
  panic("set_at: corrupt");
  return 0;
}

void set_remove(struct set * self, size_t idx) {
  size_t i = 0;
  size_t ii = 0;
  if(idx >= self->size) {
    panic("set_remove: out of bounds");
  }
  for(i = 0; i<self->capacity; i++) {
    if(self->data[i*(self->el_size+1)]) {
      if(ii == idx) {
        self->size--;
        self->data[i*(self->el_size+1)] = 0;
#if DYNAMIC_MEMORY_SUPPORT
        //free mem
#endif
        return;
      } else {
        ii++;
      }
    }
  }
  panic("set_remove: corrupt");
}

void set_remove_by_element(struct set * self, unsigned int * el) {
  size_t i = 0;
  size_t ii = 0;
  if(el - self->data >= self->capacity*(self->el_size+1)) {
    panic("set_remove_by_element: out of bounds");
  }
  self->size--;

  //fcking dangerous
  //TODO: add alignment checks
  *(el - 1) = 0;
#if DYNAMIC_MEMORY_SUPPORT
  //free mem
#endif
}

void set_clear(struct set * self) {
  size_t i = 0;
  for(i = 0; i<self->capacity; i++) {
    self->data[i*(self->el_size+1)] = 0;
  }
  self->size = 0;
#if DYNAMIC_MEMORY_SUPPORT
  //free mem
#endif

}
