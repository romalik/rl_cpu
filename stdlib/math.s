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
alloc_b 1
addrf_b 0
iaddrf_b 0
store
addrf_b 1
iaddrf_b 1
store
addrl_b 0
cnst_b 0
store
jump_w $3
.label $2
addrf_b 0
iaddrf_b 0
iaddrf_b 1
sub
store
addrl_b 0
iaddrl_b 0
add_b 1
store
.label $3
iaddrf_b 0
iaddrf_b 1
ge_w $2
iaddrl_b 0
ret
.label $1
.endproc DIV 1 0
.export MODI1
.export MODU1
.label MODI1
.label MODU1
alloc_b 1
addrf_b 0
iaddrf_b 0
store
addrf_b 1
iaddrf_b 1
store
addrl_b 0
cnst_b 0
store
jump_w $7
.label $6
addrf_b 0
iaddrf_b 0
iaddrf_b 1
sub
store
addrl_b 0
iaddrl_b 0
add_b 1
store
.label $7
iaddrf_b 0
iaddrf_b 1
ge_w $6
iaddrf_b 0
ret
.label $5
.endproc MOD 1 0
