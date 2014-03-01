#include "init.as"
#include "puts.as"
#include "readline.as"
@start
CALL2(@puts, @str0, REG_TTY)
add 10 0 REG_TTY
add 10 0 REG_TTY
CALL2(@puts, @str1, REG_TTY)
add 10 0 REG_TTY
CALL2(@puts, @str2, REG_TTY)
add 10 0 REG_TTY
CALL2(@puts, @str3, REG_TTY)
add 10 0 REG_TTY
hlt 0 0 0
jmp 0 0 @start

@str0 "mouse: "
@str1 " ()_() "
@str2 "  \\"/  "
@str3 "    *   "

@firststr "  >string read: "
@secondstr .100


