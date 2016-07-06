#include <stdio.h>
#include <vector>
#include <stdint.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <pwd.h>
#include <dirent.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
std::vector<uint16_t> image;

int dirEntryIdx = 0;
int fileAddr = 36*256;

void put_file(char *name) {
    struct stat sb;
    struct tm *t;
    char link_read[255];
    ssize_t bytes_read;
    lstat(name, &sb);

    printf("%s ", S_ISDIR(sb.st_mode) ? "directory" : "file     ");

    printf("%5.0lu ", sb.st_size);

    printf("%s\n", name);

    if (name[0] == '.') {
        printf("skip\n");
    } else if (S_ISDIR(sb.st_mode)) {
    } else {

        printf("create file %s\n", name);
        for(int i = 0; i<strlen(name); i++) {
          int dEntryPos = dirEntryIdx * 32 + 35*256 + i;
          image[dEntryPos] = name[i];
        }

        fileAddr = ((fileAddr >> 8) + 1) << 8;
        int nodeAddr = fileAddr;
        fileAddr = ((fileAddr >> 8) + 1) << 8;


        image[dirEntryIdx * 32 + 35*256 + 31] = nodeAddr >> 8;

#define FS_NONE 0
#define FS_FILE 1
#define FS_DIR 2
#define FS_CHAR_DEV 3
#define FS_BLOCK_DEV 4
#define FS_PIPE 5
#define FS_LINK 6
        image[nodeAddr] = FS_FILE;

        int cBlockOff = 3;
        int fileSize = 0;



        FILE *fd = fopen(name, "r");
        int n = 0;

        uint16_t buf[256];

        while (n = fread(buf, sizeof(uint16_t), 256, fd)) {
          image[nodeAddr + cBlockOff] = fileAddr >> 8;
          cBlockOff++;
          for(int i = 0; i<n;i++) {
            image[fileAddr] = (buf[i] >> 8) | ((buf[i]&0xff)<<8);
            fileAddr++;
            fileSize++;
          }
        }
        image[nodeAddr + 1] = fileSize;

        dirEntryIdx++;
    }
}
void get_contents(DIR *d) {
    struct dirent *entry;
    int i = 0;
    while ((entry = readdir(d)) != NULL) {
        put_file(entry->d_name);
    }
}

int main(int argc, char **argv) {
    if (argc != 4) {
        printf("Usage: make_rootfs image_file image_size root_dir\n");
        return 0;
    }
    int sz = atoi(argv[2]);

    image.resize(sz, 0);


    image[34*256] = FS_DIR;
    image[34*256 + 1] = 255;
    image[34*256 + 3] = 35;


    image[dirEntryIdx * 32 + 35*256] = '.';
    image[dirEntryIdx * 32 + 35*256 + 1] = 0;
    image[dirEntryIdx * 32 + 35*256 + 31] = 34;

    dirEntryIdx++;

    {
        DIR *d;
        int i = 3;
        struct stat s;
        char buf[255];
        lstat(argv[i], &s);
        if (S_ISDIR(s.st_mode)) {
            getwd(buf);
            chdir(argv[i]);
            printf("%s\n", argv[i]);
            d = opendir(".");
            get_contents(d);
            chdir(buf);
        }
    }

    FILE *fd = fopen(argv[1], "w");
    fwrite(image.data(), sizeof(uint16_t), sz, fd);
    fclose(fd);
    return 0;
}


