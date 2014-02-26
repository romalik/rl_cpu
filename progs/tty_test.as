#include "init.as"
#include "puts.as"

@start
CALL2(@puts, @firststr, REG_TTY);
CALL2(@puts, @secondstr, REG_TTY);
jmp 0 0 @start ;

@firststr "Hello "
@secondstr " __world__ !!!    "
