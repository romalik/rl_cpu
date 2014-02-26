#function mult
@mult
LOCAL1(REG_A)
LOCAL2(REG_B)
add 0 0 REG_C
@mult_check
je *REG_B 0 @mult_end
and *REG_B 1 REG_D
je *REG_D 0 @mult_shift
add *REG_A *REG_C REG_C
@mult_shift
shl *REG_A 0 REG_A
shr *REG_B 0 REG_B
jmp 0 0 @mult_check
@mult_end
RET()




#mult
#@mult
#add 0 0 REG_C; #40
#add 0 0 REG_D; #44
#@m_loop
#add *REG_C *REG_A REG_C; #48
#add *REG_D 1 REG_D; #4c
#jl  *REG_D *REG_B @m_loop; #50
#RET() #54 58

