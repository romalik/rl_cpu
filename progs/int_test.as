#include "init.as"

@start
@loop
add 0xeee0 0 REG_DISPLAY
add 0xeee1 0 REG_DISPLAY
add 0xeee2 0 REG_DISPLAY
add 0xeee3 0 REG_DISPLAY
add 0xeee4 0 REG_DISPLAY
add 0xeee5 0 REG_DISPLAY
add 0xeee6 0 REG_DISPLAY
add 0xeee7 0 REG_DISPLAY
add 0xeee8 0 REG_DISPLAY
add 0xeee9 0 REG_DISPLAY
jmp 0 0 @loop

@intHandler

add 0xdead 0 REG_DISPLAY
reti 0 0 0
