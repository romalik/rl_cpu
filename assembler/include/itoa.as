.seg0
@itoa
LOCAL1(REG_A)
add 10000 0 REG_C
jg *REG_A *REG_C @itoa_loop
add 1000 0 REG_C
jg *REG_A *REG_C @itoa_loop
add 100 0 REG_C
jg *REG_A *REG_C @itoa_loop
add 10 0 REG_C
jg *REG_A *REG_C @itoa_loop
add 1 0 REG_C

@itoa_loop
LOCAL1(REG_A)
TO_LOCAL1(*REG_C)
CALL2(@div, *REG_A, *REG_C)
add *REG_D 0 REG_A
LOCAL2(REG_B)
add *REG_C 0x30 *REG_B
add *REG_B 1 REG_B
TO_LOCAL2(*REG_B)
LOCAL1(REG_C)
TO_LOCAL1(*REG_A)
CALL2(@div, *REG_C, 10)
jg *REG_C 0 @itoa_loop
LOCAL2(REG_B)
add 0 0 *REG_B
RET()
