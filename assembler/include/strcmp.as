.seg0
@strcmp
LOCAL1(REG_A) #str1
LOCAL2(REG_B) #str2
@strcmp_loop
sub **REG_A **REG_B REG_C
jne *REG_C 0 @strcmp_finish
je **REG_A 0 @strcmp_finish
add *REG_A 1 REG_A
add *REG_B 1 REG_B
jmp 0 0 @strcmp_loop
@strcmp_finish
RET();
