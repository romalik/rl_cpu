#include "init.as"
#include "puts.as"
#include "div.as"
#include "mul.as"
#include "itoa.as"
#include "atoi.as"
#include "readline.as"
#include "strcmp.as"
#include "calc.as"
.seg0
@start
add 10 0 REG_TTY
CALL2(@puts, @str_ps, REG_TTY)
CALL2(@readline, REG_KB, @cmd_buf)
add 10 0 REG_TTY
CALL2(@strcmp, @cmd_buf, @cmd_calc)
je *REG_C 0 @cmd_exec_calc
CALL2(@strcmp, @cmd_buf, @cmd_echo)
je *REG_C 0 @cmd_exec_echo
CALL2(@strcmp, @cmd_buf, @cmd_hello)
je *REG_C 0 @cmd_exec_hello
CALL2(@strcmp, @cmd_buf, @cmd_null)
je *REG_C 0 @cmd_exec_null
jmp 0 0 @cmd_exec_unknown

@cmd_exec_calc
CALL(@calc_main)
jmp 0 0 @start

@cmd_exec_echo
jmp 0 0 @start

@cmd_exec_hello
CALL2(@puts, @str_hello, REG_TTY)
jmp 0 0 @start

@cmd_exec_null
jmp 0 0 @start

@cmd_exec_unknown
CALL2(@puts, @str_unknown, REG_TTY)
CALL2(@puts, @cmd_buf, REG_TTY)
jmp 0 0 @start



.seg1


@cmd_calc "calc"
@cmd_echo "echo"
@cmd_hello "hello"
@cmd_null ""
@str_hello "Hello there, man!"
@str_unknown "Bad command: "
@str_ps "cmd> "


@cmd_buf .20
