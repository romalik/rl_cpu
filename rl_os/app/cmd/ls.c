#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>

unsigned int buf[1];


int main(int argc, char **argv) {
    DIR * fd;
    struct stat st;
    struct dirent *res;
    int do_stat = 0;

    if(argc > 1) {
        if(!strcmp(argv[1], "-l")) {
                do_stat = 1;
        }
    }

    fd = opendir(".");
    res = readdir(fd);
    if(do_stat) {
            //printf("dev\tino\tmode\tnlink\tuid\tgid\trdev\tsize\tatime\tmtime\tctime\t - name\n");
            printf("type\tino\tsize\t -\tname\n");
    }

    while (res) {
        if(do_stat) {
            void * n = res->d_name;
            struct dirent * r = res;
            stat(res->d_name, &st);
            printf("%s\t%05u\t%05u\t -\t", (S_ISREG(st.st_mode)?"REG":(S_ISDIR(st.st_mode)?"DIR":(S_ISCHR(st.st_mode)?"CHR":"???"))), st.st_ino, st.st_size);

        } 
        printf("%s\n", res->d_name);
        res = readdir(fd);
    }

    closedir(fd);

    return 0;
}
