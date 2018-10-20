#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>

unsigned int buf[1];

DIR * fd;
struct stat st;
struct dirent *res;
int do_stat = 0;
int i = 0;
void do_ls(char * p);

int main(int argc, char **argv) {

    if(argc > 1) {
        if(!strcmp(argv[1], "-l")) {
                do_stat = 1;
        }
    }
    if(argc < 2 + do_stat) {
    	do_ls(".");
    } else {
	for(i = 1 + do_stat; i<argc; i++) {
	  do_ls(argv[i]);
        }
    }
    return 0;
}


char fullpath[128];

void do_ls(char * p) {
    fd = opendir(p);
    res = readdir(fd);
    if(do_stat) {
            //printf("dev\tino\tmode\tnlink\tuid\tgid\trdev\tsize\tatime\tmtime\tctime\t - name\n");
            printf("type\tino\t\tsize\t -\tname\n");
    }

    while (res) {
        if(do_stat) {
            void * n = res->d_name;
            struct dirent * r = res;
	    sprintf(fullpath, "%s/%s", p, res->d_name);
            stat(fullpath, &st);
            printf("%s\t%05u\t%10lu\t -\t", (S_ISREG(st.st_mode)?"REG":(S_ISDIR(st.st_mode)?"DIR":(S_ISCHR(st.st_mode)?"CHR":"???"))), st.st_ino, st.st_size);

        } 
        printf("%s\n", res->d_name);
        res = readdir(fd);
    }

    closedir(fd);
}
