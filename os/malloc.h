#ifndef KMALLOC_H
#define KMALLOC_H
/*
 * kmalloc.h -- Heap memory allocation routines
 * From JamesM's kernel development tutorials.
 */
#include "types.h"

void malloc_init(size_t begin, size_t end);
void *malloc(size_t len); /* classic malloc */
void free(void *ptr);

#endif /* ndef KMALLOC_H */
