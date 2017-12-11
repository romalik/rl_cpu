#include <piper.h>
#include <types.h>
#include <sched.h>
#include <lock.h>

struct pipefile {
    off_t wPos;
    off_t rPos;
    int closed;
    unsigned int mx;
    unsigned int buf[PIPE_MAX_LENGTH];
};

struct pipefile pipes[MAX_PIPES];

void piper_init() {
    int i;
    k_mkdir("/tmp");
    for(i = 0; i<MAX_PIPES; i++) {
        pipes[i].wPos = 0;
        pipes[i].rPos = 0;
        pipes[i].closed = 0;
        spinlock_init(&pipes[i].mx);
    }
}

unsigned int piper_read(unsigned int p, unsigned int * buf, size_t n) {
    //int i;
    size_t alreadyRead = 0;
    size_t readNow = 0;
    while(n) {
        while(pipes[p].rPos >= pipes[p].wPos) {
            if(pipes[p].closed) return alreadyRead;
	    //sleep(cProc, &pipes[p]);
            resched_now();
        }
	spinlock_lock(&pipes[p].mx);
        readNow = n;

        if(readNow > pipes[p].wPos - pipes[p].rPos) {
           readNow = pipes[p].wPos - pipes[p].rPos;
        }
        memcpy(buf + alreadyRead, pipes[p].buf + pipes[p].rPos, readNow);
        pipes[p].rPos += readNow;
        n-=readNow;
        alreadyRead += readNow;

        if(pipes[p].rPos == pipes[p].wPos) {
            pipes[p].rPos = 0;
            pipes[p].wPos = 0;
	    spinlock_unlock(&pipes[p].mx);
    	    return alreadyRead;
        }
	spinlock_unlock(&pipes[p].mx);
    }

    return alreadyRead;
}

unsigned int piper_write(unsigned int p, const unsigned int * buf, size_t n) {
    //int i;
    size_t alreadyWritten = 0;
    size_t writeNow = 0;
    while(n) {
         while(pipes[p].wPos >= PIPE_MAX_LENGTH) {
            //wakeup(&pipes[p]);
            resched_now();
            if(pipes[p].closed) {
                return alreadyWritten;
            }
         }

	 spinlock_lock(&pipes[p].mx);
         writeNow = n;
         if(writeNow > PIPE_MAX_LENGTH - pipes[p].wPos) {
             writeNow = PIPE_MAX_LENGTH - pipes[p].wPos;
         }

         memcpy(pipes[p].buf + pipes[p].wPos, buf, writeNow);

         pipes[p].wPos += writeNow;
         n-=writeNow;
         alreadyWritten += writeNow;
	 spinlock_unlock(&pipes[p].mx);
         //wakeup(&pipes[p]);

    }
    return alreadyWritten;
}

unsigned int piper_close(unsigned int p) {
    pipes[p].closed = 1;
    //printf("Piper close %d\n", p);

    return 0;
}
