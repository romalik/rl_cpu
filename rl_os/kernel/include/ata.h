#ifndef ATA_H__
#define ATA_H__
#include <memmap.h>
#include <kstdio.h>
#include <string.h>
#include <blkdriver.h>

#define CMD_ATA_READ 1
#define CMD_ATA_WRITE 2
#define CMD_ATA_RESET 3

void ataInit(unsigned int device);
void ataReadDataBuffer(unsigned int *Buffer, unsigned int numBytes);
void ataWriteDataBuffer(unsigned int *Buffer, unsigned int numBytes);

unsigned int ataReadSectorsLBA(unsigned int device, unsigned int lba, unsigned int *Buffer);
unsigned int ataWriteSectorsLBA(unsigned int device, unsigned int lba, unsigned int *Buffer);

struct iblkdriver * ataGetDriver();


#endif
