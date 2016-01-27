#ifndef RLFS_H__
#define RLFS_H__
#include <kstdio.h>
#include <string.h>
#include "ata.h"

#define MAX_FILES 10

/* Sector 0:
 *
 *  0:  FileID(16) FileSize(16) StartSector(16) FileName(13*16)
 *  16: FileID(16) FileSize(16) StartSector(16) FileName(13*16)
 *  ...
 *  ...
 *      FFFF
 *
 *
 *
 */

struct FileDescriptor {
    int id;
    int mode;
    int size;
    int pos;
    int posInSector;
    int baseSector;
    int currentSector;
};

void rlfs_init();

void rlfs_mkfs();

int rlfs_create(char *name);

/* returns handle */
int rlfs_open(char *name, int mode);

int rlfs_close(int fd);

int rlfs_seek(int fd, int pos);

int rlfs_write(int fd, int c);

int rlfs_read(int fd);

int rlfs_isEOF(int fd);

int rlfs_tellg(int fd);

int rlfs_removeFile(char *filename);

int rlfs_removeFile(char *filename);

#endif
