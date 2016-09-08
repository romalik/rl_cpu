#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>

#define PROCBUFSIZE 64*8
static unsigned int buf[PROCBUFSIZE];

struct pEntry {
    unsigned int pid;
    unsigned int state;
    char cmd[32];
};


char stateToString[][8] = {"Unknown", "Running", "Waiting", "Zombie", "KWorker", "New"};



void readProc(char *name) {
    int n = 0;
    int fd;
    int i;
    fd = open(name, O_RDONLY);

    n = read(fd, buf, PROCBUFSIZE);
    
    n = n/34;

    for(i = 0; i<n; i++) {
        int state;
        struct pEntry * p = (struct pEntry *)(buf + i*34);

        state = p->state;
        if(state < 1 || state > 5)
            state = 0;
        printf("%d\t%s\t\t%s\n", p->pid, stateToString[p->state], p->cmd);
    }

    close(fd);
}

int main(int argc, char **argv) {

  printf("PID\tSTATE\t\tCMD\n");
  readProc("/proc");
    return 0;
}
