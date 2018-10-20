#include "ata.h"
#include "kstdio.h"
#include <blkdriver.h>

static struct iblkdriver ataDriver;

void ataStop(unsigned int device) {}


struct iblkdriver * ataGetDriver() {
  ataDriver.init = &ataInit;
  ataDriver.read = &ataReadSectorsLBA;
  ataDriver.write = &ataWriteSectorsLBA;
  ataDriver.free = &ataStop;
  return &ataDriver;
}

void ataInit(unsigned int device) {
    outb(ATA_CONTROL_PORT, CMD_ATA_RESET);
}

void ataReadDataBuffer(unsigned int *Buffer, unsigned int numBytes) {
    unsigned int i;

    for (i = 0; i < numBytes; i++) {
        *(Buffer++) = inb(ATA_DATA_PORT);
    }
}

void ataWriteDataBuffer(unsigned int *Buffer, unsigned int numBytes) {
    unsigned int i;

    for (i = 0; i < numBytes; i++) {
        outb(ATA_DATA_PORT, *(Buffer++));
    }
}

unsigned int ataReadSectorsLBA(unsigned int device, unsigned int Sector, unsigned int *Buffer) {
    outb(ATA_CONTROL_PORT, CMD_ATA_READ);
    outb(ATA_DATA_PORT, 0);
    outb(ATA_DATA_PORT, (Sector & 0xffff));
    ataReadDataBuffer(Buffer, 256);
    return 0;
}

unsigned int ataWriteSectorsLBA(unsigned int device, unsigned int Sector, unsigned int *Buffer) {
    outb(ATA_CONTROL_PORT, CMD_ATA_WRITE);
    outb(ATA_DATA_PORT, 0);
    outb(ATA_DATA_PORT, (Sector & 0xffff));
    ataWriteDataBuffer(Buffer, 256);
    return 0;
}

