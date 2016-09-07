#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

int main(int argc, char **argv) {
    int pid;
    int sig = SIGKILL;
    if(argc < 2) {
        printf("Usage: %s pid [sig]\n", argv[0]);
        return 0;
    }

    if(argc > 2) {
        sig = atoi(argv[2]);
    }
    pid = atoi(argv[1]);

    printf("Killing pid %d with signal %d\n", pid, sig);
    kill(pid,sig);

    return 0;
}
