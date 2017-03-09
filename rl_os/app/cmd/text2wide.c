#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>


int main(int argc, char ** argv) {
    int i;
    int in;
    int out;
    unsigned int v;
    if(argc < 2) {
        printf("Usage: %s file_in file_out\n", argv[0]);
    }
    
    in = open(argv[1], O_RDONLY);
    out = open(argv[2], O_WRONLY);

    while(read(in, &v, 1)) {
            unsigned int vs[2];
            vs[0] = (v >> 8) & 0xff;
            vs[1] = v & 0xff;
            write(out, vs, 2);
    }

    close(in);
    close(out);

    return 0;
}
