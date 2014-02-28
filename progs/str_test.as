#include "init.as"
#include "puts.as"
#include "div.as"
#include "mul.as"
#include "itoa.as"
#include "atoi.as"
@start
CALL2(@puts, @str1, REG_TTY)
CALL2(@puts, @str3, REG_TTY)
CALL1(@atoi, @str3)
CALL2(@itoa, *REG_C, @buf)
CALL2(@puts, @str2, REG_TTY)
CALL2(@puts, @buf, REG_TTY)
jmp 0 0 @start

@str1 "   > string: "
@str2 " atoi+itoa: "
@str3 "1357"
@buf .100


