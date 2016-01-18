#include <stdio.h>

int main(int argc, char ** argv) {
  int i;
    int pid = 0;
    printf("Hello from userland!\n");
    while(1) {
    printf("Try to fork!\n");
    pid = fork();
    printf("Pid : %d\n", pid);
    }
    return 0;
}
