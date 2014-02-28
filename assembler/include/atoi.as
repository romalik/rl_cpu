@atoi
LOCAL1(REG_A)
add 0 0 REG_C
add 0 0 REG_D
@atoi_loop
jl **REG_A 0x30 @atoi_end
jg **REG_A 0x39 @atoi_end
TO_LOCAL1(*REG_A)
CALL2(@mult, *REG_C, 10)
LOCAL1(REG_A)
sub **REG_A 0x30 REG_E
add *REG_C *REG_E REG_C
add *REG_A 1 REG_A
jmp 0 0 @atoi_loop
@atoi_end
RET()
