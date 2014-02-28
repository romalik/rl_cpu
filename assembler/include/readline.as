@readline
LOCAL1(REG_A) #source port
LOCAL2(REG_B) #dest address of first char
add 0 0 REG_C
@readline_next_char
add **REG_A 0 *REG_B
je **REG_B 0 @readline_next_char
je **REG_B 10 @readline_finish
add *REG_B 1 REG_B
add *REG_C 1 REG_C
add *REG_C 0 REG_DISPLAY
jmp 0 0 @readline_next_char
@readline_finish
add 0 0 *REG_B
RET();
