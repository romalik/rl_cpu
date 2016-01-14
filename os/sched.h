#ifndef SCHED_H__
#define SCHED_H__
#include <types.h>
#include <memmap.h>


#define PROC_STATE_NONE 0
#define PROC_STATE_RUN 1
#define PROC_STATE_WAIT 2
#define PROC_STATE_EXIT 3
unsigned int ticks = 0;

struct Process {
  unsigned int pid;
  unsigned int state;
  unsigned int ap;
  unsigned int bp;
  unsigned int sp;
  unsigned int pc;

  unsigned int memBank;
};

#define MAXPROC 15




#endif
