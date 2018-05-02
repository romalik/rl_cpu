#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

char str[] = "Write/read test\n";

int main(int argc, char **argv, char ** envp) {
    int i = 0;
    int j = 0;
    int cnt = 0;
    char * c;

    i = fork();
    if(i) {
      while(1) {
        printf("Parent %d\n", cnt);
        cnt++;
        for(j = 0; j<10000; j++) {

        }
      }

    } else {
      while(1) {
        printf("Child %d\n", cnt);
        cnt++;
        for(j = 0; j<10000; j++) {

        }
      }

    }


    printf("Echo: argc = %d\n", argc);

    printf("envp = 0x%04x, environ = 0x%04x\n", (size_t)envp, (size_t)environ);

    for (i = 0; i < argc; i++) {
        printf("%d: %s atoi %d\n", i, argv[i], atoi(argv[i]));
    }


    c = getenv("blah");
    printf("getenv blah= [%s]\n", c);
    setenv("blah", "BLAH_VALUE", 0);

    c = getenv("blah");
    printf("getenv blah= [%s]\n", c);
    setenv("blah", "BLAH_VALUE", 0);

    c = getenv("blah");
    printf("getenv blah= [%s]\n", c);
    unsetenv("blah");
    
    c = getenv("blah");
    printf("getenv blah= [%s]\n", c);
    return 0;
}
