#include "init.as"
#include "mul.as"




@start
add 0 0x3a REG_A
add 0 0xeb REG_B
CALL2(@mult, *REG_A, *REG_B)
add *REG_C 0 REG_DISPLAY
hlt 0 0 0

