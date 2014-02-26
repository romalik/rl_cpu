#define __SP 0x1010
#define REG_A 0x1000
#define REG_B 0x1001
#define REG_C 0x1002
#define REG_D 0x1003
#define REG_DISPLAY 0xfffe
#define REG_DISPLAY1 0xfffd
#define REG_TTY 0xfffc
#define REG_KB 0xffff
#include "defs.inc"

add 0xF000 0 __SP; #set up stack pointer
jmp 0 0 @start
