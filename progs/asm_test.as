#define __SP 0x1010
#define REG_A 0x1000
#define REG_B 0x1001
#define REG_C 0x1002
#define REG_D 0x1003
#define REG_DISPLAY 0xfffe
#include "../assembler/defs.inc"

add 0xdead 0 REG_DISPLAY
add 0 0 REG_B
add @string 0 REG_A
@check
add **REG_A 0 REG_DISPLAY
je **REG_A 0 @end
add *REG_A 1 REG_A
add *REG_B 1 REG_B
jmp 0 0 @check
@end
jmp 0 0 @end

@string "hello"

#add 0x1000 0 __SP; #set up stack pointer
#add 2 0 REG_A;  #04
#add 3 0 REG_B;  #08
#CALL(@mult)   #0c 10 14 18
#add *REG_C 0 REG_DISPLAY #1c
#add 3 0 REG_A;  #20
#add 5 0 REG_B;  #24
#CALL(@mult)    #28 2c 30 34
#add *REG_C 0 REG_DISPLAY #38
#jmp 0 0 0x3c #3c


#mult
#@mult
#add 0 0 REG_C; #40
#add 0 0 REG_D; #44
#add *REG_C *REG_A REG_C; #48
#add *REG_D 1 REG_D; #4c
#jl  *REG_D *REG_B 0x48; #50
#RET() #54 58

