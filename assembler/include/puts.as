@puts
LOCAL1(REG_A)
LOCAL2(REG_B)
@puts_loop
je **REG_A 0 @puts_end
add **REG_A 0 *REG_B
add *REG_A 1 REG_A
jmp 0 0 @puts_loop
@puts_end
RET();
