#ifndef SCHED_H__
#define SCHED_H__
#include <types.h>
#include <memmap.h>
#include <rlfs3.h>
#include <signal.h>


#define MAXPROC 15
#define MAX_FILES_PER_PROC 15

#define PROC_STATE_NONE 0
#define PROC_STATE_RUN 1
#define PROC_STATE_SLEEP 2
#define PROC_STATE_ZOMBIE 3
#define PROC_STATE_KWORKER 4
#define PROC_STATE_NEW 5
#define PROC_STATE_CONSTRUCT 6
#define PROC_STATE_BLOCKED 7

extern unsigned int ticks;

/*
 Interrupt:
            this->push(highPC());
            this->push(BP);
            this->push(AP);
            this->push(S);
            this->push(D);
            this->push(SW());

 */

/*
 Status word:
        //15 14    13    12     11 10 09 08 07 06 05 04 03 02 01 00
        //-- MMUEn User  IntEn  ----mPC---- ------mmuSelector------

        w res = 0;
        res |= (MMUEntrySelector & 0xff);
        res |= ((mPC()) << 8);
        res |= (intEnabled << 12);
        res |= (userMode << 13);
*/


#define SW_COMMIT_ENABLE (1<<14)
//#define SW_MMU_ENABLE (1<<14)
#define SW_INT_ENABLE (1<<12)
#define SW_USER_MODE  (1<<13)


struct InterruptFrame { //userspace
  size_t SW;
  size_t SP;
  size_t highPC;
  size_t BP;
  size_t AP;
  size_t S;
  size_t D;
};

struct Process {
  unsigned int pid;
  unsigned int state;

  struct InterruptFrame intFrame;


  int mode;

  unsigned int mmuSelector;

  struct fs_node cwd;

  struct Process *parent;

  int retval;

  FILE *openFiles[MAX_FILES_PER_PROC];

  char  * argv;
  char cmd[32];
  int signalsPending;
  sighandler_t sigActions[SIGNUM];
  void * waitingOn;
  int isThread;
};


void printProcess(struct Process *p);
void resched(unsigned int t_stack[]);

extern struct Process *cProc;
extern struct Process procs[MAXPROC];

/*
struct IntFrame {
    unsigned int sp;
    unsigned int pc;
    unsigned int mpc;
    unsigned int bp;
    unsigned int ap;
    unsigned int s;
    unsigned int d;
};
*/


struct Process *get_free_proc();
void run_proc(struct Process * p);


unsigned int sendSig(unsigned int pid, unsigned int sig);

unsigned int findProcByPid(unsigned int pid, struct Process **p);
//struct Process *sched_add_proc(unsigned int pid, unsigned int mmuSelector,
//                               struct Process *p);
unsigned int sched_genPid();

void resched_now();

unsigned int proc_file_read(unsigned int minor, unsigned int * buf, size_t n);
unsigned int proc_file_write(unsigned int minor, const unsigned int * buf, size_t n);

unsigned int sched_file_read(unsigned int minor, unsigned int * buf, size_t n);
unsigned int sched_file_write(unsigned int minor, const unsigned int * buf, size_t n);

void sleep(struct Process * proc, void * event);
void wakeup(void * event);

#endif
