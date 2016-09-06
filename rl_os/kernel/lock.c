#include <lock.h>

void spinlock_init(unsigned int * l) {
    *l = 0;
}

void spinlock_lock(unsigned int * l) {
    while(test_and_set(l) == 1) {}
}

void spinlock_unlock(unsigned int * l) {
    *l = 0;
}
