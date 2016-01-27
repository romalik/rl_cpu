#ifndef ATA_H__
#define ATA_H__
#include <memmap.h>
#include <kstdio.h>
#include <string.h>

#define CMD_ATA_READ 1
#define CMD_ATA_WRITE 2
#define CMD_ATA_RESET 3

void ataInit(void);
void ataReadDataBuffer(unsigned int *Buffer, unsigned int numBytes);
void ataWriteDataBuffer(unsigned int *Buffer, unsigned int numBytes);

unsigned char ataReadSectorsLBA(unsigned int lba, unsigned int *Buffer);
unsigned char ataWriteSectorsLBA(unsigned int lba, unsigned int *Buffer);

unsigned char ataReadSectors(unsigned int lba, unsigned int *Buffer,
                             unsigned long *actualBuffer);
unsigned char ataWriteSectors(unsigned int lba, unsigned int *Buffer);

#endif
