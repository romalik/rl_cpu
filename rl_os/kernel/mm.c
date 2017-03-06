#include <sched.h>
#include <mm.h>
#include <memmap.h>

unsigned int BankMap[NBANKS];

void mm_init() {
    int i;
    for (i = 0; i < NBANKS; i++) {
        BankMap[i] = 0;
    }
}
void mm_freeSegment(unsigned int seg) {
    BankMap[seg] = 0;
}

unsigned int mm_allocSegment(unsigned int *seg) {
    int i;
    for (i = 0; i < NBANKS; i++) {
        if (BankMap[i] == 0) {
            BankMap[i] = 1;
            *seg = i;
            return 1;
        }
    }
    return 0;
}
void mm_memcpy(unsigned int *dest, unsigned int *src, unsigned int n,
               unsigned int bank) {
    DATA_BANK_SEL = bank;
    memcpy(dest, src, n);
}
