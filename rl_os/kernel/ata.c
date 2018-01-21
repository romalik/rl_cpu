#include "ata.h"
#include "kstdio.h"
void ataInit() {
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

unsigned char ataReadSectorsLBA(unsigned int Sector, unsigned int *Buffer) {
    outb(ATA_CONTROL_PORT, CMD_ATA_READ);
    outb(ATA_DATA_PORT, 0);
    outb(ATA_DATA_PORT, (Sector & 0xffff));
    ataReadDataBuffer(Buffer, 256);
    return 0;
}

unsigned char ataWriteSectorsLBA(unsigned int Sector, unsigned int *Buffer) {
    outb(ATA_CONTROL_PORT, CMD_ATA_WRITE);
    outb(ATA_DATA_PORT, 0);
    outb(ATA_DATA_PORT, (Sector & 0xffff));
    ataWriteDataBuffer(Buffer, 256);
    return 0;
}

unsigned char ataReadSectors(unsigned int lba, unsigned int *Buffer,
                             unsigned long *SectorInCache) {
    unsigned int cyl, head, sect;
    unsigned char temp;

    if (*SectorInCache == lba)
        return 0;

    *SectorInCache = lba;

    temp = ataReadSectorsLBA(lba, Buffer);

    return temp;
}

unsigned char ataWriteSectors(unsigned int lba, unsigned int *Buffer) {
    unsigned int cyl, head, sect;
    unsigned char temp;

    temp = ataWriteSectorsLBA(lba, Buffer);
    return temp;
}
