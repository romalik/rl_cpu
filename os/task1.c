#include <stdio.h>
#include <fork.h>
#include <exec.h>
int main(int argc, char **argv) {
    int i = 0;
    int pid = 0;
    printf("Try to fork!\n");
    pid = fork();
    if (pid == 0) {
        printf("I am a child!\n");
        execve((unsigned int *)"/bin/hello", 0, 0);
    } else {
        printf("I am a parent. Child pid : %d\n", pid);
        while (1) {
            int j = 0;
            printf("Hello from parent! %d\n", i);
            i++;
            for (j = 0; j < 10000; j++) {
                j--;
                j++;
            }
        }
    }
    return 123;
}
