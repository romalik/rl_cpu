#include <kstdio.h>
#include <string.h>
#include "sh_builtin.h"
#include "ata.h"
#include "rlfs3.h"
#include "types.h"
#include "sched.h"
#include "kernel_worker.h"
#include <mm.h>
//#include <vfs.h>
#include <blk.h>
#include <tty.h>
#include <piper.h>


extern char __data_end;
extern char __code_end;
extern void __timer_interrupt_vector();
extern void __system_interrupt_vector();

extern void syscall();
extern void ei();
#define TIMER_INTERRUPT_ADDR_PORT INT3_vec
#define SYSTEM_INTERRUPT_ADDR_PORT INT0_vec

char init_path[] = "/sh";

void init_interrupts() {
    outb(TIMER_INTERRUPT_ADDR_PORT, (size_t)(__timer_interrupt_vector));
    outb(SYSTEM_INTERRUPT_ADDR_PORT, (size_t)(__system_interrupt_vector));
    ei();
}
int blah() {return 0;}


int kernel_main() {


    // malloc_init((size_t)&__data_end, (size_t)(0x3000));
    //  printf("Init interrupts..\n");

    ataInit();
    block_init();
    fs_init();


    mmu_test();
    //while(1) {};

    mmu_init();
    sched_init();
    piper_init();

    kernel_worker_init();

    k_regDevice(0, tty_write, tty_read, 0, 0);
    k_regDevice(1, proc_file_write, proc_file_read, 0, 0);
    k_regDevice(2, piper_write, piper_read, 0, piper_close);
    k_regDevice(3, sched_file_write, sched_file_read, 0, 0);

    k_mkdir("/dev");

    k_mknod("/dev/tty", 'c', 0, 0);
    k_mknod("/dev/proc", 'c', 1, 0);
    k_mknod("/dev/schedctl", 'c', 3, 0);

    printf("Press s for builtin shell, any key for init [%s]\n", init_path);

    init_interrupts();


    if (getc() == 's') {
        main_sh();
    } else {
        struct execSyscall initExecRequest;
        initExecRequest.id = __NR_execve;
        initExecRequest.filename = (void *)init_path;
        initExecRequest.argv = (void *)init_path;
        initExecRequest.envp = NULL;
        addKernelTask(KERNEL_TASK_FORK, 0, 0); //will fork to pids 0 & 1
        addKernelTask(KERNEL_TASK_EXECVE, 1, (void *)(&initExecRequest));
        procs[0].openFiles[0] = k_open("/dev/tty", 'r');
        procs[0].openFiles[1] = k_open("/dev/tty", 'w');
        procs[0].openFiles[2] = k_open("/dev/tty", 'w');
        sched_start();
        kernel_worker();
        /*
        struct Process * initP;
        unsigned int b;
        unsigned int c;
        int initPid = 0;
        mm_allocSegment(&b);
        mm_allocSegment(&c);
        printf("Loading init\n");

        initPid = sched_genPid();
        initP = sched_add_proc(initPid, b, c, 0);

        if(do_exec(initP, INIT_PATH, NULL, NULL)) {
            printf("Failed to exec init!\n");
            halt();
        }


        printf("Starting scheduler\n");
        sched_start();
        while (1) {
        }
*/
    }

    printf("System halted\n");
    while (1) {
    }
    return 0;
}
