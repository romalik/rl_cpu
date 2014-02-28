@atoi
LOCAL1(REG_A)
add 0 0 REG_C
@atoi_loop
je **REG_A 0 @atoi_end
TO_LOCAL1(*REG_A)
CALL2(@mult, *REG_C, 10)
LOCAL1(REG_A)
sub **REG_A 0x30 REG_D
add *REG_C *REG_D REG_C
add *REG_A 1 REG_A
jmp 0 0 @atoi_loop
@atoi_end
RET()
