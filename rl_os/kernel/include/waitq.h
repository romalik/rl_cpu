#ifndef WAITQ_H__
#define WAITQ_H__

#include <types.h>
#include <sched.h>
#include <rlfs3.h>
#include <cb.h>

#define WAITQ_LENGTH 15
#define WAITQ_PENDING_LENGTH 200

#define WAITQ_TYPE_NONE 0
#define WAITQ_TYPE_FILE 1
#define WAITQ_TYPE_WAITPID 2
#define WAITQ_TYPE_TIMER 3

struct waitqEntry {
	unsigned int trigger;
	pid_t pid;
	unsigned int type;
	size_t scallStruct;
	unsigned int params[10];
	
};

extern struct waitqEntry waitq[WAITQ_LENGTH];
extern struct circular_buffer waitqPendingNotifications;

unsigned int waitq_init();
unsigned int waitqGetNextWaiter(unsigned int what, struct waitqEntry ** res);
unsigned int waitqAddEntry(pid_t who, unsigned int what, unsigned int type, size_t scallStruct, unsigned int * params);
unsigned int waitqNotify(unsigned int what);


#endif

