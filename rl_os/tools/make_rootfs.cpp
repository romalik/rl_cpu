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
int fileAddr = 80*256;

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

#define M_S_IFMT      0170000
#define M_S_IFSOCK    0140000     /* Reserved, not used */
#define M_S_IFLNK     0120000     /* Reserved, not used */
#define M_S_IFREG     0100000
#define M_S_IFBLK     0060000
#define M_S_IFDIR     0040000
#define M_S_IFCHR     0020000
#define M_S_IFIFO     0010000

#define M_S_ISUID     0004000
#define M_S_ISGID     0002000
#define M_S_ISVTX     0001000     /* Reserved, not used */
#define M_S_IRWXU     0000700
#define M_S_IRUSR     0000400
#define M_S_IWUSR     0000200
#define M_S_IXUSR     0000100
#define M_S_IRWXG     0000070
#define M_S_IRGRP     0000040
#define M_S_IWGRP     0000020
#define M_S_IXGRP     0000010
#define M_S_IRWXO     0000007
#define M_S_IROTH     0000004
#define M_S_IWOTH     0000002
#define M_S_IXOTH     0000001
        
        image[nodeAddr] = M_S_IFREG;
        {
          int usedBlock = nodeAddr >> 8;
          int cBitmap = usedBlock >> 12;
          int cSect = usedBlock & 0xfff;
          int idx = cSect >> 4;
          int pos = usedBlock & 0x0f;
          image[(cBitmap+1)*256 + idx] |= (1<<pos);
        }
        int cBlockOff = 3;
        int fileSize = 0;



        FILE *fd = fopen(name, "r");
        int n = 0;

        uint16_t buf[256];

        while (n = fread(buf, sizeof(uint16_t), 256, fd)) {
          int usedBlock = fileAddr >> 8;
          int cBitmap = usedBlock >> 12;
          int cSect = usedBlock & 0xfff;
          int idx = cSect >> 4;
          int pos = usedBlock & 0x0f;
          image[(cBitmap+1)*256 + idx] |= (1<<pos);

          image[nodeAddr + cBlockOff] = fileAddr >> 8;
          cBlockOff++;
          for(int i = 0; i<n;i++) {
            image[fileAddr] = (buf[i] >> 8) | ((buf[i]&0xff)<<8);
            fileAddr++;
            fileSize++;
          }
        }
        image[nodeAddr + 1] = fileSize;
        image[nodeAddr + 2] = fileSize>>16;

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


    image[34*256] = M_S_IFDIR;
    image[34*256 + 3] = 35;
    image[34*256 + 4] = 36;
    image[34*256 + 5] = 37;
    image[34*256 + 6] = 38;
    image[34*256 + 7] = 39;
    image[34*256 + 8] = 40;
    image[34*256 + 9] = 41;
    image[34*256 + 10] = 42;
    image[34*256 + 11] = 43;
    image[34*256 + 12] = 44;
    image[34*256 + 13] = 45;
    image[34*256 + 14] = 46;


    image[dirEntryIdx * 32 + 35*256] = '.';
    image[dirEntryIdx * 32 + 35*256 + 1] = 0;
    image[dirEntryIdx * 32 + 35*256 + 31] = 34;

    dirEntryIdx++;

    image[dirEntryIdx * 32 + 35*256] = '.';
    image[dirEntryIdx * 32 + 35*256 + 1] = '.';
    image[dirEntryIdx * 32 + 35*256 + 2] = 0;
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

    int rootEntrySz = dirEntryIdx * 32;
    image[34*256 + 1] = rootEntrySz;//(rootEntrySz >> 8) | ((rootEntrySz &0xff) << 8);


    for(int i = 256; i<290; i++) {
      image[i] = 0xffff;
    }


    FILE *fd = fopen(argv[1], "w");
    fwrite(image.data(), sizeof(uint16_t), sz, fd);
    fclose(fd);
    return 0;
}


