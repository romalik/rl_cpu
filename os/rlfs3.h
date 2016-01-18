#ifndef RLFS_H__
#define RLFS_H__
#include <stdio.h>
#include <string.h>
#include "ata.h"
#include <vfs.h>
#define MAX_FILES 10


/*  Block 0: Directory root
 *
 */


/*  Dir:
 *
 *  0:  FileName(15*16) FileHeaderPtr(16)
 *  16:  FileName(15*16) FileHeaderPtr(16)
 *  ...
 *  ...
 *      FFFF
 */

/* File header:
 *  attrs(16)
 *  size(16)
 *  254 block indexes
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



void rlfs2_init();

void rlfs2_mkfs();

int rlfs2_create(char * name);

/* returns handle */
int rlfs2_open(char * name, int mode);


int rlfs2_close(int fd);


int rlfs2_seek(int fd, int pos);


int rlfs2_write(int fd, int c);


int rlfs2_read(int fd);

int rlfs2_isEOF(int fd);

int rlfs2_tellg(int fd);

int rlfs2_removeFile(char * filename);

int rlfs2_removeFile(char * filename);

#endif
