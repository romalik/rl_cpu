#include "ata.h"
#include "kstdio.h"
#include <blkdriver.h>

static struct iblkdriver ataDriver;

static unsigned int ata_ctl_ports[2] =
{
  ATA_CONTROL_PORT,
  ATA_2_CONTROL_PORT
};

static unsigned int ata_data_ports[2] =
{
  ATA_DATA_PORT,
  ATA_2_DATA_PORT
};

void ataStop(unsigned int device) {}


struct iblkdriver * ataGetDriver() {
  ataDriver.init = &ataInit;
  ataDriver.read = &ataReadSectorsLBA;
  ataDriver.write = &ataWriteSectorsLBA;
  ataDriver.free = &ataStop;
  return &ataDriver;
}

void ataInit(unsigned int device) {
    outb(ata_ctl_ports[device], CMD_ATA_RESET);
}

void ataReadDataBuffer(unsigned int device, unsigned int *Buffer, unsigned int numBytes) {
    unsigned int i;

    for (i = 0; i < numBytes; i++) {
        *(Buffer++) = inb(ata_data_ports[device]);
    }
}

void ataWriteDataBuffer(unsigned int device, unsigned int *Buffer, unsigned int numBytes) {
    unsigned int i;

    for (i = 0; i < numBytes; i++) {
        outb(ata_data_ports[device], *(Buffer++));
    }
}

unsigned int ataReadSectorsLBA(unsigned int device_full, unsigned int Sector, unsigned int *Buffer) {
  unsigned int device = DEV_MINOR(device_full);
    outb(ata_ctl_ports[device], CMD_ATA_READ);
    outb(ata_data_ports[device], 0);
    outb(ata_data_ports[device], (Sector & 0xffff));
    ataReadDataBuffer(device, Buffer, 256);
    return 0;
}

unsigned int ataWriteSectorsLBA(unsigned int device_full, unsigned int Sector, unsigned int *Buffer) {
  unsigned int device = DEV_MINOR(device_full);
    outb(ata_ctl_ports[device], CMD_ATA_WRITE);
    outb(ata_data_ports[device], 0);
    outb(ata_data_ports[device], (Sector & 0xffff));
    ataWriteDataBuffer(device, Buffer, 256);
    return 0;
}

