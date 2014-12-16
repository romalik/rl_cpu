#include "init.as"
#include "puts.as"
#include "div.as"
#include "mul.as"
#include "itoa.as"
#include "atoi.as"
#include "readline.as"

@calc
add 0 0xffff @currOp

LOCAL1(REG_A)

@calcLoop
CALL1(@atoi, *REG_A)
LOCAL2(REG_B)

add *REG_B 0 REG_DISPLAY
je *@currOp 0 @calcLoopOut
je *@currOp 0xffff @currOp_FIRST
je *@currOp 0x2b @currOp_PLUS
je *@currOp 0x2d @currOp_MINUS
je *@currOp 0x2a @currOp_MUL
je *@currOp 0x2f @currOp_DIV
jmp 0 0 @calcLoopOut


@currOp_FIRST
add 0 *REG_C REG_B
jmp 0 0 @currOp_BREAK

@currOp_PLUS
add *REG_B *REG_C REG_B
jmp 0 0 @currOp_BREAK

@currOp_MINUS
sub *REG_B *REG_C REG_B
jmp 0 0 @currOp_BREAK

@currOp_BREAK
TO_LOCAL2(*REG_B)

LOCAL1(REG_A)


@calc_skip_num
jl **REG_A 0x30 @calc_skip_num_end
jg **REG_A 0x39 @calc_skip_num_end
add *REG_A 1 REG_A
jmp 0 0 @calc_skip_num
@calc_skip_num_end


add **REG_A 0 @currOp
add *REG_A 1 REG_A
TO_LOCAL1(*REG_A)
jmp 0 0 @calcLoop

@calcLoopOut

add *REG_B 0 REG_C
RET()


@start
CALL2(@puts, @str1, REG_TTY)
CALL2(@readline, REG_KB, @buf)
CALL2(@puts, @str2, REG_TTY)
CALL2(@calc, @buf, 0)
CALL2(@itoa, *REG_C, @buf)
CALL2(@puts, @buf, REG_TTY)
add 10 0 REG_TTY
jmp 0 0 @start

@currOp .1
@str1 "   > "
@str2 " = "
@buf .100


