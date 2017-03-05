#ifndef __MEMMAP_H
#define __MEMMAP_H

#define ATA_CONTROL_PORT (*(unsigned int *)(0x7ffc))
#define ATA_DATA_PORT (*(unsigned int *)(0x7ffd))

#define PORT_OUT (*(unsigned int *)(0x7fff))
#define UART (*(unsigned int *)(0x7ffe))

#define INT0_vec (*(unsigned int *)(0x7fea))
#define INT1_vec (*(unsigned int *)(0x7feb))
#define INT2_vec (*(unsigned int *)(0x7fec))
#define INT3_vec (*(unsigned int *)(0x7fed))
#define INT4_vec (*(unsigned int *)(0x7fee))
#define INT5_vec (*(unsigned int *)(0x7fef))
#define INT6_vec (*(unsigned int *)(0x7ffa))
#define INT7_vec (*(unsigned int *)(0x7ffb))

//#define BANK_SEL (*(unsigned int *)(0x7fdf)) = (*(unsigned int *)(0x7fde))
#define CODE_BANK_SEL (*(unsigned int *)(0x7fde))
#define DATA_BANK_SEL (*(unsigned int *)(0x7fdf))


#endif
