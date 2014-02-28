#include "init.as"
#include "puts.as"
#include "readline.as"
@start
add 0xaaaa 0 REG_DISPLAY
CALL2(@readline, REG_KB, @secondstr)
add 0xdead 0 REG_DISPLAY
CALL2(@puts, @firststr, REG_TTY)
CALL2(@puts, @secondstr, REG_TTY)
jmp 0 0 @start
/*
CALL2(@puts, @firststr, REG_TTY);
CALL2(@puts, @secondstr, REG_TTY);
*/
@pad .3
@firststr "  >string read: "
@secondstr .100


