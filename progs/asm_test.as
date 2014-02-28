#include "init.as"
#include "mul.as"
#include "div.as"




@start
add 0 0x06 REG_A
add 0 0x03 REG_B
CALL2(@mult, *REG_A, *REG_B)
add *REG_C 0 REG_DISPLAY
add 0 0x07 REG_A
add 0 0x03 REG_B
CALL2(@div, *REG_A, *REG_B)
add *REG_C 0 REG_DISPLAY
add *REG_D 0 REG_DISPLAY1
hlt 0 0 0

