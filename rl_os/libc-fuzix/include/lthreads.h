#ifndef LTHREADS_H
#define LTHREADS_H
#include "sys/types.h"
#include "malloc.h"

#define DEFAULT_STACK_SIZE 64*4


typedef struct lthread {
    void * stack_ptr;
    int id;
} lthread_t;



int lthread_create(lthread_t * lthread, void *(fn)(void *), void *arg);
int lthread_join(lthread_t * lthread);
int lthread_exit();

#endif /* ndef KMALLOC_H */
