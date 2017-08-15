#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char **argv) {

    mkfifo("/tfifo", 0777);

    return 0;
}
