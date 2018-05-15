#include <piper.h>
#include <types.h>
#include <sched.h>
#include <lock.h>
#include <cb.h>

struct pipefile {
    int used;
	int readClosed;
	int writeClosed;
    unsigned int buf[PIPE_MAX_LENGTH];
	struct circular_buffer cb;
	FILE * openedAs;
};

struct pipefile pipes[MAX_PIPES];

void piper_init() {
    int i;
    k_mkdir("/tmp");
    for(i = 0; i<MAX_PIPES; i++) {
		cb_create_static(PIPE_MAX_LENGTH, &(pipes[i].cb), pipes[i].buf);
        pipes[i].used = 0;
		pipes[i].readClosed = 0;
		pipes[i].writeClosed = 0;
    }
}

int piper_getFreePipe() {
	int i = 0;
	for(i = 0; i<MAX_PIPES; i++) {
		if(!pipes[i].used) return i;
	}
	panic("no free pipes!");
	return -1;
}

unsigned int piper_read(unsigned int p, unsigned int * buf, size_t n) {
    size_t sz_read;

	if(pipes[p].cb.size == pipes[p].cb.capacity) {
		waitqNotify((size_t)(pipes[p].openedAs->node.idx));
		//printf("Notify on read for 0x%04X\n", (size_t)(pipes[p].openedAs->node.idx));
	}
	
//	printf("piper_read : cb.size %d n %d\n", pipes[p].cb.size, n);
	
	sz_read = n;
	if(n > pipes[p].cb.size) {
		sz_read = pipes[p].cb.size;
	}
	
	n = sz_read;
	while(n) {
		*buf = cb_pop(&pipes[p].cb);
		buf++;
		n--;
	}
	
	if(!sz_read) {
		if(!pipes[p].writeClosed) {
			blockRequest = 1;
		}
	}
	
    return sz_read;
}

unsigned int piper_write(unsigned int p, const unsigned int * buf, size_t n) {
    size_t sz_write;

	if(pipes[p].readClosed) {
		blockRequest = 2;
		return 0;
	}


	if(pipes[p].cb.size == 0) {
		waitqNotify((size_t)(pipes[p].openedAs->node.idx));
		//printf("Notify on write for 0x%04X\n", (size_t)(pipes[p].openedAs->node.idx));
	}
//	printf("piper_write : n %d\n", n);
	sz_write = n;
	if(n > pipes[p].cb.capacity - pipes[p].cb.size) {
		sz_write = pipes[p].cb.capacity - pipes[p].cb.size;
	}
	
	n = sz_write;
	while(n) {
		cb_push(&pipes[p].cb, *buf);
		buf++;
		n--;
	}
	
	if(!sz_write) {
		blockRequest = 1;
	} else {
		
	}
	
    return sz_write;
}

unsigned int piper_close(unsigned int p, FILE * fd) {
	if(fd->mode == O_RDONLY) {
		//closing read end
		pipes[p].readClosed = 1;
	} else if(fd->mode == O_WRONLY) {
		//closing write end
		pipes[p].writeClosed = 1;
	}
	if(pipes[p].readClosed && pipes[p].writeClosed) {
	    pipes[p].used = 0;
		cb_clear(&pipes[p].cb);
		pipes[p].readClosed = 0;
		pipes[p].writeClosed = 0;
	}
	
    //printf("Piper close %d\n", p);

    return 0;
}

unsigned int piper_open(unsigned int p, FILE * fd) {
	pipes[p].used = 1;
	pipes[p].openedAs = fd;
	
	
    return 0;
}
