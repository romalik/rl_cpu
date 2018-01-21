#ifndef __MEMMAP_H
#define __MEMMAP_H

#define ATA_CONTROL_PORT (0x60)
#define ATA_DATA_PORT (0x61)

#define PORT_OUT (0xA0)
#define UART (0x40)

#define INT0_vec (0x20)
#define INT1_vec (0x21)
#define INT2_vec (0x22)
#define INT3_vec (0x23)
#define INT4_vec (0x24)
#define INT5_vec (0x25)
#define INT6_vec (0x26)
#define INT7_vec (0x27)

//#define BANK_SEL (*(unsigned int *)(0x7fdf)) = (*(unsigned int *)(0x7fde))
#define CODE_BANK_SEL (*(unsigned int *)(0x7fdf))
#define DATA_BANK_SEL (*(unsigned int *)(0x7fde))


#endif
