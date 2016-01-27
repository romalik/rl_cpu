#include "rlfs.h"
#include <kstdio.h>
#include <blk.h>

struct FileDescriptor openFiles[MAX_FILES];

unsigned int WORK_BUFFER[64 * 4];

void rlfs_init() {
    int i;
    for (i = 0; i < MAX_FILES; i++) {
        openFiles[i].id = 0xffff;
    }
}

void rlfs_mkfs() {
    int i;
    struct Block *b;
    b = bread(0, 0);
    for (i = 0; i < 256; i++) {
        b->data[i] = 0;
    }
    b->flags = BLOCK_MODIFIED;
    bfree(b);

    b = bread(0, 1);
    b->data[0] = 0x3;
    b->flags = BLOCK_MODIFIED;
    bfree(b);
}

int rlfs_findFreeSector() {
    int i;
    int j;
    struct Block *b;
    printf("Alloc sect\n");
    b = bread(0, 1);
    for (i = 0; i < 256; i++) {
        if (b->data[i] != 0xffff) {
            for (j = 0; j < 16; j++) {
                if ((b->data[i] & (1 << j)) == 0) {
                    printf("sect: %d\n", i * 16 + j);
                    return i * 16 + j;
                }
            }
        }
    }
    bfree(b);
    return 0;
}

void rlfs_markSector(int sect, int val) {
    int idx;
    int pos;
    struct Block *b;
    b = bread(0, 1);
    idx = sect >> 4;
    pos = sect & 0x0f;
    if (val) {
        b->data[idx] = (b->data[idx] | (1 << pos));
    } else {
        b->data[idx] = (b->data[idx] & ~(1 << pos));
    }
    b->flags = BLOCK_MODIFIED;
    bfree(b);
}

int rlfs_create(char *name) {
    int freeSect;
    int i;
    struct Block *b;
    printf("create file %s\n", name);
    freeSect = rlfs_findFreeSector();
    rlfs_markSector(freeSect, 1);

    b = bread(0, 0);
    for (i = 0; i < 256; i += 16) {
        if (b->data[i] == 0 || b->data[i] == 0xffff)
            break;
    }
    b->data[i] = 1;
    b->data[i + 1] = 0;
    b->data[i + 2] = freeSect;
    strcpy((char *)(b->data) + 3 + i, (char *)(name));
    bfree(b);
    printf("created file at %d\n", freeSect);
    return i;
}

void removeMarkersForFile(unsigned int cSector, unsigned int cSize) {
    struct Block *b;
    while (cSize > 255) {
        b = bread(0, cSector);
        cSector = b->data[4 * 64 - 1];
        rlfs_markSector(cSector, 0);
        cSize -= 255;
        bfree(b);
    }
}

/* returns handle */
int rlfs_open(char *name, int mode) {
    int fd;
    int i;
    struct Block *b;

    /* find free descriptor */
    for (fd = 0; fd < MAX_FILES; fd++) {
        if (openFiles[fd].id == 0xffff) {
            break;
        }
    }
    b = bread(0, 0);
    for (i = 0; i < 256; i += 16) {
        if (!strcmp((char *)(b->data) + i + 3, name) &&
            (b->data[i] != 0xffff)) {
            break;
        }
    }
    if (i == 256) {
        if (mode == 'w') {
            i = rlfs_create(name);
        } else {
            bfree(b);
            return -1;
        }
    }
    openFiles[fd].baseSector = openFiles[fd].currentSector = b->data[i + 2];
    openFiles[fd].size = b->data[i + 1];
    openFiles[fd].id = i >> 4;
    openFiles[fd].mode = mode;
    openFiles[fd].pos = 0;
    openFiles[fd].posInSector = 0;

    printf("File %s opened: baseSector %d size %d\n", name,
           openFiles[fd].baseSector, openFiles[fd].size);

    if (mode == 'w') {
        /* clear all sector marks for this file */
        removeMarkersForFile(openFiles[fd].baseSector, openFiles[fd].size);
        openFiles[fd].size = 0;
    }
    bfree(b);
    return fd;
}

int rlfs_removeFile(char *filename) {
    int fd;
    int id;
    int sect;
    int size;
    struct Block *b;
    fd = rlfs_open(filename, 'w');
    if (fd < 0) {
        return -1;
    }
    id = openFiles[fd].id;
    sect = openFiles[fd].baseSector;
    size = openFiles[fd].size;
    rlfs_close(fd);
    b = bread(0, 0);
    b->data[id << 4] = 0xffff;
    b->flags = BLOCK_MODIFIED;
    bfree(b);
    removeMarkersForFile(sect, size);
    return 0;
}

int rlfs_close(int fd) {
    int entryPos;
    struct Block *b;
    entryPos = openFiles[fd].id << 4;
    b = bread(0, 0);
    b->data[entryPos + 1] = openFiles[fd].size;
    openFiles[fd].id = 0xffff;
    b->flags = BLOCK_MODIFIED;
    bfree(b);
    return 0;
}

int rlfs_getNextSector(fd) {
    int res;
    struct Block *b;
    b = bread(0, openFiles[fd].currentSector);
    res = b->data[64 * 4 - 1];
    bfree(b);
    return res;
}

int rlfs_seek(int fd, int pos) {
    int cPos;
    cPos = pos;
    if (pos > openFiles[fd].size) {
        return -1;
    }
    while (cPos > 255) {
        openFiles[fd].currentSector = rlfs_getNextSector(fd);
        cPos -= 255;
    }
    openFiles[fd].pos = pos;
    openFiles[fd].posInSector = cPos;
    return 0;
}

int rlfs_write(int fd, int c) {
    struct Block *b;
    b = bread(0, openFiles[fd].currentSector);
    b->data[openFiles[fd].posInSector] = c;
    b->flags = BLOCK_MODIFIED;
    openFiles[fd].pos++;
    openFiles[fd].posInSector++;

    if (openFiles[fd].pos < openFiles[fd].size) {
        int cSec = openFiles[fd].currentSector;
        if (openFiles[fd].posInSector == 255) {
            openFiles[fd].currentSector = b->data[64 * 4 - 1];
            openFiles[fd].posInSector = 0;
        }
    } else {
        openFiles[fd].size++;
        if (openFiles[fd].posInSector == 255) {
            int newSector = rlfs_findFreeSector();
            b->data[64 * 4 - 1] = newSector;
            openFiles[fd].currentSector = newSector;
            openFiles[fd].posInSector = 0;
            rlfs_markSector(newSector, 1);
        }
    }
    bfree(b);
}

int rlfs_read(int fd) {
    int retval;
    struct Block *b;
    b = bread(0, openFiles[fd].currentSector);
    if (openFiles[fd].pos < openFiles[fd].size) {
        retval = b->data[openFiles[fd].posInSector];
        openFiles[fd].pos++;
        openFiles[fd].posInSector++;
        if (openFiles[fd].posInSector == 255) {
            openFiles[fd].currentSector = b->data[64 * 4 - 1];
            openFiles[fd].posInSector = 0;
        }
        bfree(b);
        return retval;
    } else {
        bfree(b);
        return 0;
    }
}

int rlfs_isEOF(int fd) {
    if (openFiles[fd].pos == openFiles[fd].size)
        return 1;
    else
        return 0;
}

int rlfs_tellg(int fd) {
    return openFiles[fd].size;
}
