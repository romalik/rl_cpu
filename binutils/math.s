.code
.label LEI2
iaddrf_b 1 ; load A_high
iaddrf_b 3 ; load B_high
gt_w lei2_ret_0
iaddrf_b 0 ; load A_low
iaddrf_b 2 ; load B_low
gt_w lei2_ret_0
cnst_b 1
cnst_b 0
ret2
.label lei2_ret_0
cnst_b 0
cnst_b 0
ret2



.code
.label MULI1
	
cnst_b 0                       ;here we keep the result, accessable as local 1
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



.label skip

iaddrf_b 1                     ;load B again
lsh_b 1                                ;shift it left
addrf_b 1                      ;store it back
rstore

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
alloc_b 5
alloc_b 1
addrl_b 0+5
iaddrf_b 0
iaddrf_b 1
cnst_w DIVI1
call_w fastcall2
addrs_w -4
rstore
discard_b 2
store
iaddrf_b 0 ;load A
iaddrl_b 0+5 ;load C
iaddrf_b 1 ;load B
cnst_w MULI1
call_w fastcall2 ;C*B
addrs_w -4
rstore
discard_b 2
sub ; A - C*B
ret

.endproc MOD 1 0
