#define __SP 0x1010
#define REG_A 0x1000
#define REG_B 0x1001
#define REG_C 0x1002
#define REG_D 0x1003
#define REG_E 0x1004
#define REG_F 0x1005
#define REG_LOCAL 0x1006
#define REG_DISPLAY 0xfffe
#define REG_DISPLAY1 0xfffd
#define REG_TTY 0xfffc
#define REG_KB 0xffff
#include "defs.inc"

add 0xF000 0 __SP; #set up stack pointer 0x00
jmp 0 0 @start     #                     0x04

hlt 0 0 0 #0x08
hlt 0 0 0 #0x0c
hlt 0 0 0 #0x10

hlt 0 0 0 #0x14
hlt 0 0 0 #0x18
hlt 0 0 0 #0x1c
jmp 0 0 @intHandler #0x20


