#include <waitq.h>



struct waitqEntry waitq[WAITQ_LENGTH];

struct circular_buffer waitqPendingNotifications;

unsigned int waitqPendingNotificationsArena[WAITQ_PENDING_LENGTH];

unsigned int waitq_init() {
	int i = 0;
	for(i = 0; i<WAITQ_LENGTH; i++) {
		waitq[i].type = WAITQ_TYPE_NONE;
	}
	cb_create_static(WAITQ_PENDING_LENGTH, &waitqPendingNotifications, waitqPendingNotificationsArena);
	return 0;
}


unsigned int waitqGetNextWaiter(unsigned int what, struct waitqEntry ** res) {
	int found = 0;

	if(!(*res)) *res = &waitq[0];
	//printf("waitqGetWaiter() called\n");
	while(*res != &waitq[WAITQ_LENGTH]) {
		if((*res)->type != WAITQ_TYPE_NONE) {
			if((*res)->trigger == what) {
				found = 1;
				break;
			}
		}
		(*res)++;
	}
	return found;
}

unsigned int waitqAddEntry(pid_t who, unsigned int what, unsigned int type, size_t scallStruct, unsigned int * params) {
	int i = 0;
	for(i = 0; i<WAITQ_LENGTH; i++) {
		if(waitq[i].type == WAITQ_TYPE_NONE) {
			break;
		}
	}

	//printf("waitqAddEntry : caller %d waitfor 0x%04X\n", who, what);
	
	if(i == WAITQ_LENGTH) {
		panic("waitq overflow\n");
	}
	
	waitq[i].pid = who;
	waitq[i].trigger = what;
	waitq[i].type = type;
	waitq[i].scallStruct = scallStruct;
	if(type == WAITQ_TYPE_FILE) {
		waitq[i].params[0] = params[0]; //fd
		waitq[i].params[1] = params[1]; //dest
		waitq[i].params[2] = params[2]; //n
		waitq[i].params[3] = params[3]; //r/w
	} else if(type == WAITQ_TYPE_TIMER) {

	} else if(type == WAITQ_TYPE_WAITPID) {
		
	}
	return 0;
	
}
unsigned int waitqNotify(unsigned int what) {

	if(waitqPendingNotifications.size < waitqPendingNotifications.capacity) {
		cb_push(&waitqPendingNotifications, what);
	} else {
		panic("waitqPendingNotifications overflow!");
	}
	return 0;
}


