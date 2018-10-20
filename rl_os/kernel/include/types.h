#ifndef TYPES_H
#define TYPES_H

typedef unsigned long uint32_t;
typedef long int32_t;
typedef unsigned int uint16_t;
typedef int int16_t;
typedef unsigned char uint8_t;
typedef char int8_t;

typedef unsigned int size_t;

typedef unsigned long off_t;
typedef unsigned int blk_t;

typedef int16_t pid_t;

extern void panic(char * s);

#define DEV_MAJOR(x) ((x) >> 8)
#define DEV_MINOR(x) ((x) & 0xff)

#define NULL 0

#define RPC_CALLBACK_ON_READ 0
#define RPC_CALLBACK_ON_WRITE 1
#define RPC_CALLBACK_ON_OPEN 2
#define RPC_CALLBACK_ON_CLOSE 3
#define RPC_CALLBACK_ON_IOCTL 4
#define RPC_CALLBACK_TYPES_NR 5


#endif /* ndef TYPES_H */
