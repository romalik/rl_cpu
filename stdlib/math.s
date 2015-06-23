.code
.label MULI1
cnst_b 0                       ;here we keep the result, accessable as local 0
.label mult_begin
iaddrf_b 0                     ;load A
cnst_b 0                       ;compare with 0
eq_w mult_end          ;A == 0 - jump to end

iaddrf_b 0                     ;load A again
band_b 1                       ;check LSB
cnst_b 0                       ;if equals to zero
eq_w skip                      ;jump to shift
                                       ;else
iaddrl_b 0                     ;load current result
iaddrf_b 1                     ;load current B
add                                    ;sum them
addrl_b 0                      ;store new result
rstore                         ;

iaddrf_b 1                     ;load B again
lsh_b 1                                ;shift it left
addrf_b 1                      ;store it back
rstore


.label skip
iaddrf_b 0                     ;load A
rsh_b 1                                ;shift it right
addrf_b 0                      ;store A back
rstore

jump_w mult_begin

.label mult_end
ret


.export DIVI1
.export DIVU1
.code 
.label DIVI1
.label DIVU1

alloc_b 2
addrf_b 0
iaddrf_b 0
store
addrf_b 1
iaddrf_b 1
store
addrl_b 1
cnst_b 0
store
addrl_b 0
cnst_b 0
store
jump_w $3
.label $2
addrl_b 1
iaddrl_b 1
add_b 1
store
addrl_b 0
iaddrl_b 0
iaddrf_b 1
add
store
.label $3
iaddrl_b 0
iaddrf_b 0
le_w $2
addrl_b 1
iaddrl_b 1
sub_b 1
store
iaddrl_b 1
ret
.label $1



.endproc DIV 1 0
.export MODI1
.export MODU1
.label MODI1
.label MODU1

alloc_b 2

;iaddrf_b 0 ;load A


addrl_b 0 ;A -> arg0
iaddrf_b 0
store

addrl_b 1 ;B ->arg 1
iaddrf_b 1
store

call_w DIVI1 ;Q = A/B

addrl_b 0
rstore ;Q -> arg0

addrl_b 1
iaddrf_b 1 ;load B
store ;B -> arg 1

call_w MULI1 ;Z = B*Q

iaddrf_b 0

sub ;res = A - Z
ret


.endproc MOD 1 0
