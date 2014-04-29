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

@calc_main
CALL2(@puts, @calc_disclaimer, REG_TTY)
add 10 0 REG_TTY

@calc_main_loop
CALL2(@puts, @calc_str1, REG_TTY)
CALL2(@readline, REG_KB, @calc_buf)
CALL2(@strcmp, @calc_buf, @calc_exit_str)
je *REG_C 0 @calc_exit
CALL2(@puts, @calc_str2, REG_TTY)
CALL2(@calc, @calc_buf, 0)
CALL2(@itoa, *REG_C, @calc_buf)
CALL2(@puts, @calc_buf, REG_TTY)
add 10 0 REG_TTY
jmp 0 0 @calc_main_loop
@calc_exit
RET()
.seg1


@calc_disclaimer "---=== Calculator v0.1 for rlcpu ===---"
@calc_str1 "calc> "
@calc_str2 " = "
@calc_exit_str "exit"
@calc_buf .10
@currOp .1


