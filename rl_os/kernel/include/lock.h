#ifndef LOCK_H_
#define LOCK_H_

extern unsigned int test_and_set(unsigned int * p);

void spinlock_init(unsigned int * l);
void spinlock_lock(unsigned int * l);
void spinlock_unlock(unsigned int * l);


#endif
