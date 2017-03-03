#include <stdio.h>
#include <lthreads.h>

int global_var = 0;

void * worker1(void * arg) {
    int i, j;
    for(i = 0; i<100; i++) {
        printf("worker1: %d\n", i);
        global_var++;
        for(j = 0; j<10000; j++) {
        }
    }

    lthread_exit();
    return 0;
}

void * worker2(void * arg) {
    int i, j;
    for(i = 0; i<100; i++) {
        printf("worker2: %d\n", i);
        global_var++;
        for(j = 0; j<10000; j++) {
        }
    }

    lthread_exit();
    return 0;
}


int main() {
    lthread_t t1, t2;
    int i, j;

    lthread_create(&t1, &worker1, 0);
    lthread_create(&t2, &worker2, 0);

    for(i = 0; i<50; i++) {
        printf("main: %d\n", i);
        global_var++;
        for(j = 0; j<10000; j++) {
        }
    }



    lthread_join(&t1);
    lthread_join(&t2);

    printf("Joined!\n");
    printf("global_var: %d (should be 250)\n", global_var);
    return 0;
}
