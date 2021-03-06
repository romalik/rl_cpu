#include <kstdio.h>
#include <string.h>
#include "sh_builtin.h"
#include "ata.h"
#include "rlfs3.h"
#include "malloc.h"
#include "types.h"
#include "sched.h"
#include "kernel_worker.h"
#include <mm.h>
//#include <vfs.h>
#include <blk.h>
#include <tty.h>

extern char __data_end;
extern char __code_end;
extern void __timer_interrupt_vector();
extern void __system_interrupt_vector();

extern void syscall();

#define TIMER_INTERRUPT_ADDR_PORT INT3_vec
#define SYSTEM_INTERRUPT_ADDR_PORT INT0_vec

#define INIT_PATH "/sh"

void init_interrupts() {
    TIMER_INTERRUPT_ADDR_PORT = (size_t)(__timer_interrupt_vector);
    SYSTEM_INTERRUPT_ADDR_PORT = (size_t)(__system_interrupt_vector);
    ei();
}

int kernel_main() {
    // malloc_init((size_t)&__data_end, (size_t)(0x3000));
    //  printf("Init interrupts..\n");

    init_interrupts();

    ataInit();
    block_init();
    fs_init();
    mm_init();
    sched_init();
    kernel_worker_init();

    k_regDevice(0, tty_write, tty_read);

    k_mknod("/tty", 'c', 0, 0);

    printf("Press s for builtin shell, any key for init [%s]\n", INIT_PATH);

    if (getc() == 's') {
        main_sh();
    } else {
        unsigned int b;
        int initPid = 0;
        FILE *fd1 = k_open(INIT_PATH, 'r');
        size_t cPos = 0x8000;

        mm_allocSegment(&b);
        BANK_SEL = b;
        printf("Loading init\n");
        while (!k_isEOF(fd1)) {
            cPos += k_read(fd1, (unsigned int *)cPos, fd1->size);
        }
        k_close(fd1);

        initPid = sched_genPid();
        sched_add_proc(initPid, b, 0);

        procs[initPid].openFiles[0] = k_open("/tty", 'r');
        procs[initPid].openFiles[1] = k_open("/tty", 'w');
        procs[initPid].openFiles[2] = k_open("/tty", 'w');

        printf("Starting scheduler\n");
        sched_start();
        while (1) {
        }
    }

    printf("System halted\n");
    while (1) {
    }
    return 0;
}
