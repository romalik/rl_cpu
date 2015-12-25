#ifndef ATA_H__
#define ATA_H__
#include <stdio.h>
#include <string.h>



#define CMD_ATA_READ 1
#define CMD_ATA_WRITE 2
#define CMD_ATA_RESET 3

#define ATA_CONTROL_PORT (*(unsigned int *)(0xfffc))
#define ATA_DATA_PORT (*(unsigned int *)(0xfffd))


void            ataInit             (void);
void            ataReadDataBuffer   (unsigned char *Buffer, unsigned int numBytes);
void            ataWriteDataBuffer  (unsigned char *Buffer, unsigned int numBytes);

unsigned char   ataReadSectorsLBA      (unsigned int lba, unsigned char *Buffer);
unsigned char   ataWriteSectorsLBA     (unsigned int lba, unsigned char *Buffer);

unsigned char   ataReadSectors      (unsigned int lba, unsigned char *Buffer, unsigned long * actualBuffer);
unsigned char   ataWriteSectors     (unsigned int lba, unsigned char *Buffer);

#endif