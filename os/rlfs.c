#include "rlfs.h"


struct FileDescriptor openFiles[MAX_FILES];


unsigned char WORK_BUFFER[64*4];

void rlfs_init() {
  int i;
  for(i = 0; i<MAX_FILES; i++) {
    openFiles[i].id = 0xffff;
  }

}

void rlfs_mkfs() {
  int i;
  for(i = 0; i<256; i++) {
    WORK_BUFFER[i] = 0;
  }
  ataWriteSectorsLBA(0, WORK_BUFFER);
  WORK_BUFFER[0] = 0x3;
  ataWriteSectorsLBA(1, WORK_BUFFER);

}

int rlfs_findFreeSector() {
  int i;
  int j;
  ataReadSectorsLBA(1, WORK_BUFFER);
  for(i = 0; i<256; i++) {
    if(WORK_BUFFER[i] != 0xffff) {
      for(j = 0; j<16; j++) {
        if((WORK_BUFFER[i] & (1 << j)) == 0) {
          return i*16 + j;
        }
      }
    }
  }
  return 0;

}

void rlfs_markSector(int sect, int val) {
  int idx;
  int pos;
  ataReadSectorsLBA(1, WORK_BUFFER);
  idx = sect >> 4;
  pos = sect & 0x0f;

  if(val) {
    WORK_BUFFER[idx] = (WORK_BUFFER[idx] | (1<<pos));
  } else {
    WORK_BUFFER[idx] = (WORK_BUFFER[idx] & ~(1<<pos));
  }
  ataWriteSectorsLBA(1, WORK_BUFFER);
}

int rlfs_create(char * name) {
  int freeSect;
  int i;
  freeSect = rlfs_findFreeSector();
  rlfs_markSector(freeSect, 1);

  ataReadSectorsLBA(0, WORK_BUFFER);
  for(i = 0; i<256; i+=16) {
    if(WORK_BUFFER[i] == 0)
      break;
  }
  WORK_BUFFER[i] = 1;
  WORK_BUFFER[i+1] = 0;
  WORK_BUFFER[i+2] = freeSect;
  strcpy((char *)(WORK_BUFFER)+3, (char *)(name));
  return 0;
}

/* returns handle */
int rlfs_open(char * name, int mode) {
  int fd;
  int i;
  /* find free descriptor */
  for(fd = 0; fd<MAX_FILES; fd++) {
    if(openFiles[fd].id == 0xffff) {
      break;
    }
  }
  ataReadSectorsLBA(0, WORK_BUFFER);
  for(i = 0; i<256; i+=16) {
    if(!memcmp(WORK_BUFFER + i + 3, name)) {
      openFiles[fd].baseSector = openFiles[fd].currentSector = WORK_BUFFER[i+2];
      openFiles[fd].size = WORK_BUFFER[i+1];
      openFiles[fd].id = i>>4;
      openFiles[fd].mode = mode;
      openFiles[fd].pos = 0;
      openFiles[fd].posInSector = 0;
      break;
    }
  }
  if(mode == 'w') {
    /* clear all sector marks for this file */
  }
  return fd;

}


int rlfs_close(int fd) {
  int entryPos;
  entryPos = openFiles[fd].id << 4;
  ataReadSectorsLBA(0, WORK_BUFFER);
  WORK_BUFFER[entryPos + 1] = openFiles[fd].size;
  openFiles[fd].id = 0xffff;
  return 0;
}

int rlfs_getNextSector(fd) {
  ataReadSectorsLBA(openFiles[fd].currentSector, WORK_BUFFER);
  return WORK_BUFFER[64*4-1];
}

int rlfs_seek(int fd, int pos) {
  int cPos;
  cPos = pos;
  if(pos > openFiles[fd].size) {
    return -1;
  }
  while(cPos > 254) {
    openFiles[fd].currentSector = rlfs_getNextSector(fd);
    cPos -= 254;
  }
  openFiles[fd].pos = pos;
  openFiles[fd].posInSector = cPos;
  return 0;
}


int rlfs_write(int fd, int c) {
  ataReadSectorsLBA(openFiles[fd].currentSector, WORK_BUFFER);
  WORK_BUFFER[openFiles[fd].pos] = c;
  openFiles[fd].pos++;
  openFiles[fd].posInSector++;
  if(openFiles[fd].pos < openFiles[fd].size) {
    ataWriteSectorsLBA(openFiles[fd].currentSector, WORK_BUFFER);
    if(openFiles[fd].posInSector == 255) {
      openFiles[fd].currentSector = WORK_BUFFER[64*4-1];
      openFiles[fd].posInSector = 0;
    }
  } else {
    openFiles[fd].size++;
    ataWriteSectorsLBA(openFiles[fd].currentSector, WORK_BUFFER);
    if(openFiles[fd].posInSector == 255) {
      int newSector = rlfs_findFreeSector();
      ataReadSectorsLBA(openFiles[fd].currentSector, WORK_BUFFER);
      WORK_BUFFER[64*4-1] = newSector;
      ataWriteSectorsLBA(openFiles[fd].currentSector, WORK_BUFFER);
      openFiles[fd].currentSector = newSector;
      openFiles[fd].posInSector = 0;
    }
  }
}


int rlfs_read(int fd) {
  int retval;
  ataReadSectorsLBA(openFiles[fd].currentSector, WORK_BUFFER);
  if(openFiles[fd].pos < openFiles[fd].size) {

    retval = WORK_BUFFER[openFiles[fd].pos];
    openFiles[fd].pos++;
    openFiles[fd].posInSector++;
    if(openFiles[fd].posInSector == 255) {
      openFiles[fd].currentSector = WORK_BUFFER[64*4-1];
      openFiles[fd].posInSector = 0;
    }
  } else {
    return 0;
  }

}

int rlfs_isEOF(int fd) {
  if(openFiles[fd].pos == openFiles[fd].size)
    return 1;
  else
    return 0;
}

int rlfs_tellg(int fd) {
  return openFiles[fd].size;
}
