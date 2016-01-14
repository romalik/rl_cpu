#include <sched.h>
#include <memmap.h>

unsigned int sched_stack[2*64];
unsigned sched_active = 0;


struct Process procs[MAXPROC];
unsigned int currentTask = 0;
struct Process * cProc;
void sched_init() {
  int i = 0;
  for(i = 0; i<MAXPROC; i++) {
    procs[i].state = PROC_STATE_NONE;
  }
  procs[0].state = PROC_STATE_RUN;

}

void sched_add_proc(unsigned int pid, unsigned int bank) {
  int i = 0;
  for(i = 0; i<MAXPROC; i++) {
    if(procs[i].state == PROC_STATE_NONE) {
      break;
    }
  }

  if(i == MAXPROC)
    return;

  procs[i].pid = pid;
  procs[i].memBank = bank;
  procs[i].ap = 0xC000;
  procs[i].bp = 0xC000;
  procs[i].sp = 0xC000;
  procs[i].pc = 0x8000;
  procs[i].state = PROC_STATE_RUN;

}

void sched_start() {
  currentTask = 0;
  sched_active = 1;
}

void timer_interrupt(unsigned int * ap, unsigned int * bp, unsigned int * pc, unsigned int * sp) {
  int nextTask;
  ticks++;

  if(sched_active) {
    nextTask = currentTask + 1;
    while(nextTask != currentTask) {
      if(nextTask == MAXPROC)
        nextTask = 0;

      if(procs[nextTask].state == PROC_STATE_RUN)
        break;

      nextTask++;

    }

    procs[currentTask].ap = *ap;
    procs[currentTask].bp = *bp;
    procs[currentTask].sp = *sp;
    procs[currentTask].pc = *pc;

    *ap = procs[nextTask].ap;
    *bp = procs[nextTask].bp;
    *sp = procs[nextTask].sp;
    *pc = procs[nextTask].pc;
    BANK_SEL = procs[nextTask].memBank;
    currentTask = nextTask;
    cProc = &(procs[nextTask]);
  }
}
