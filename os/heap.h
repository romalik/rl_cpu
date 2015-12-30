/*---------------------------------------------------------------- */
/* Statically-allocated memory manager */
/* */
/* by Eli Bendersky (eliben@gmail.com) */
/*   */
/* This code is in the public domain. */
/*---------------------------------------------------------------- */
#ifndef HEAP_H__
#define HEAP_H__

void malloc_init(void * begin, unsigned int size);

/* 'malloc' clone */
/* */
void* malloc(unsigned int sz);

/* 'free' clone */
/* */
void free(void* ap);



#endif /* MEMMGR_H */
