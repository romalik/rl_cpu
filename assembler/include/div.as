/*
if D == 0 then throw DivisionByZeroException end
Q := 0                 initialize quotient and remainder to zero
R := 0
for i = n-1...0 do     where n is number of bits in N
  R := R << 1          left-shift R by 1 bit
  R(0) := N(i)         set the least-significant bit of R equal to bit i of the numerator
  if R >= D then
    R = R - D
    Q(i) := 1
  end
end
*/


.seg0
#function div
@div
LOCAL1(REG_A)  #N
LOCAL2(REG_B)  #D
add  0 0 REG_C #Q
add  0 0 REG_D #R
add 0x8000 0 REG_E
@div_bitsearch_loop
and *REG_E *REG_A REG_F
jne *REG_F 0 @div_loop
shr *REG_E 0 REG_E
jmp 0 0 @div_bitsearch_loop

@div_loop
shl *REG_D 0 REG_D

and *REG_D 0xfffe REG_D #reset LSB
and *REG_A *REG_E REG_F #test N(i)
je *REG_F 0 @div_skip_setbit
or *REG_D 1 REG_D
@div_skip_setbit
jl *REG_D *REG_B @div_skip_sub
sub *REG_D *REG_B REG_D
or *REG_C *REG_E REG_C
@div_skip_sub
shr *REG_E 0 REG_E
jne *REG_E 0 @div_loop
RET()
