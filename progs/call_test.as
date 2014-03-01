#define __SP 0x1010
#define REG_A 0x1000
#define REG_B 0x1001
#define REG_C 0x1002
#define REG_D 0x1003
#define REG_DISPLAY 0xfffe
#define REG_DISPLAY2 0xfffd
#define REG_TTY 0xfffc
#include "defs.inc"


add 0x1100 0 __SP; #set up stack pointer
add 0 0 REG_DISPLAY
CALL1(@shownumrec, 0x10)
#CALL(@shownum)

add 0xeeee 0 REG_DISPLAY
hlt 0 0 0


@shownumrec
LOCAL1(REG_B)
add *__SP 0 REG_DISPLAY2
add 0 *REG_B REG_DISPLAY
sub *REG_B 1 REG_B
je *REG_B 0 @quitshow
CALL1(@shownumrec, *REG_B)
@quitshow
add *__SP 0 REG_DISPLAY2
RET()
