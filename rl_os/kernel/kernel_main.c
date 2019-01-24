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
#include <blkdriver.h>
#include <tty.h>
#include <piper.h>
#include <loop.h>

extern char __data_end;
extern char __code_end;
extern void __timer_interrupt_vector();
extern void __system_interrupt_vector();
extern void __uart_interrupt_vector();
extern void __uart2_interrupt_vector();
extern void __mmu_interrupt_vector();
extern void __decoder_interrupt_vector();

extern void syscall();
extern void ei();
#define UART2_INTERRUPT_ADDR_PORT INT7_vec
#define DECODER_INTERRUPT_ADDR_PORT INT6_vec
#define MMU_INTERRUPT_ADDR_PORT INT5_vec
#define UART_INTERRUPT_ADDR_PORT INT4_vec
#define TIMER_INTERRUPT_ADDR_PORT INT3_vec
#define SYSTEM_INTERRUPT_ADDR_PORT INT0_vec

char init_path[] = "/sh";

void init_interrupts() {
  asm("ec");
  outb(DECODER_INTERRUPT_ADDR_PORT, (size_t)(__decoder_interrupt_vector));
  outb(MMU_INTERRUPT_ADDR_PORT, (size_t)(__mmu_interrupt_vector));
  outb(UART_INTERRUPT_ADDR_PORT, (size_t)(__uart_interrupt_vector));
  outb(UART2_INTERRUPT_ADDR_PORT, (size_t)(__uart2_interrupt_vector));
  outb(TIMER_INTERRUPT_ADDR_PORT, (size_t)(__timer_interrupt_vector));
  outb(SYSTEM_INTERRUPT_ADDR_PORT, (size_t)(__system_interrupt_vector));
  asm("ei");
  return;
}
int blah() {return 0;}


int kernel_main() {


    // malloc_init((size_t)&__data_end, (size_t)(0x3000));
    //  printf("Init interrupts..\n");

  ataInit(0);
  ataInit(1);
    blkdrivers_init();

    regBlkDriver(0, ataGetDriver());

    loop_init(0);
    regBlkDriver((7UL<<8), loop_get_driver());

    block_init();
    fs_init();

    uart_init();

    //mmu_test();
    //while(1) {};

    mmu_init();
    sched_init();
    piper_init();

	waitq_init();

    kernel_worker_init();



    k_regDevice(0, 0,0,0,0,0);
    k_regDevice(1, tty_write, tty_read, tty_open, tty_close, tty_ioctl);
    k_regDevice(2, proc_file_write, proc_file_read, 0, 0, 0);
    k_regDevice(3, piper_write, piper_read, piper_open, piper_close, 0);
    k_regDevice(4, sched_file_write, sched_file_read, 0, 0, 0);
    k_regDevice(5, null_file_write, null_file_read, 0, 0, 0);
    k_regDevice(6, zero_file_write, zero_file_read, 0, 0, 0);
    k_regDevice(7, 0, 0, 0, 0, loop_ioctl);

    k_mkdir("/dev");

    k_mknod("/dev/tty", 'c', 1, 0);
    k_mknod("/dev/tty2", 'c', 1, 1);
    k_mknod("/dev/proc", 'c', 2, 0);
    k_mknod("/dev/schedctl", 'c', 4, 0);
    k_mknod("/dev/null", 'c', 5, 0);
    k_mknod("/dev/zero", 'c', 6, 0);

    k_mknod("/dev/hda", 'b', 0, 0);
    k_mknod("/dev/hdb", 'b', 0, 1);

    k_mknod("/dev/loop", 'b', 7, 0);

//    printf("Press s for builtin shell, any key for init [%s]\n", init_path);



    if (0/*getc() == 's'*/) {
        main_sh();
    } else {
        struct execSyscall initExecRequest;
        initExecRequest.id = __NR_execve;
        initExecRequest.filename = (void *)init_path;
        initExecRequest.argv = NULL;//(void *)init_path;
        initExecRequest.envp = NULL;
        addKernelTask(KERNEL_TASK_FORK, 0, 0); //will fork to pids 0 & 1
        addKernelTask(KERNEL_TASK_EXECVE, 1, (void *)(&initExecRequest));
        procs[0].openFiles[0] = k_open("/dev/tty", 'r');
        procs[0].openFiles[1] = k_open("/dev/tty", 'w');
        procs[0].openFiles[2] = k_open("/dev/tty", 'w');
        sched_start();
        init_interrupts();
        kernel_worker();
    }

    printf("System halted\n");
    while (1) {
    }
    return 0;
}
