#include "defs.inc"

.seg0
add 0x1FFF 0 __SP; #set up stack pointer 0x00
jmp 0 0 @start     #                     0x04

hlt 0 0 0 #0x08
hlt 0 0 0 #0x0c
hlt 0 0 0 #0x10

hlt 0 0 0 #0x14
hlt 0 0 0 #0x18
hlt 0 0 0 #0x1c
jmp 0 0 @intHandler #0x20


