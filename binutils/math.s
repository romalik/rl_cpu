;;;;;;;;; UNSIGNED 2 WORD COMPARISON ;;;;;;;;;;;;;;
.code
.label LEU2
iaddrf_b 1 ; load A_high
iaddrf_b 3 ; load B_high
ugt_w leu2_ret_0
iaddrf_b 0 ; load A_low
iaddrf_b 2 ; load B_low
ugt_w leu2_ret_0
cnst_b 1
cnst_b 0
ret2
.label leu2_ret_0
cnst_b 0
cnst_b 0
ret2


.code
.label LTU2
iaddrf_b 1 ; load A_high
iaddrf_b 3 ; load B_high
ugt_w ltu2_ret_0
iaddrf_b 0 ; load A_low
iaddrf_b 2 ; load B_low
uge_w ltu2_ret_0
cnst_b 1
cnst_b 0
ret2
.label ltu2_ret_0
cnst_b 0
cnst_b 0
ret2


.code
.label GEU2
iaddrf_b 1 ; load A_high
iaddrf_b 3 ; load B_high
ult_w geu2_ret_0
iaddrf_b 0 ; load A_low
iaddrf_b 2 ; load B_low
ult_w geu2_ret_0
cnst_b 1
cnst_b 0
ret2
.label geu2_ret_0
cnst_b 0
cnst_b 0
ret2


.code
.label GTU2
iaddrf_b 1 ; load A_high
iaddrf_b 3 ; load B_high
ult_w gtu2_ret_0
iaddrf_b 0 ; load A_low
iaddrf_b 2 ; load B_low
ule_w gtu2_ret_0
cnst_b 1
cnst_b 0
ret2
.label gtu2_ret_0
cnst_b 0
cnst_b 0
ret2

;;;;;;;;;;;;;;;;;;; SIGNED 2 WORD COMPARISON ;;;;;;;;;;;;;;;;;;;

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
.label LTI2
iaddrf_b 1 ; load A_high
iaddrf_b 3 ; load B_high
gt_w lti2_ret_0
iaddrf_b 0 ; load A_low
iaddrf_b 2 ; load B_low
ge_w lti2_ret_0
cnst_b 1
cnst_b 0
ret2
.label lti2_ret_0
cnst_b 0
cnst_b 0
ret2


.code
.label GEI2
iaddrf_b 1 ; load A_high
iaddrf_b 3 ; load B_high
lt_w gei2_ret_0
iaddrf_b 0 ; load A_low
iaddrf_b 2 ; load B_low
lt_w gei2_ret_0
cnst_b 1
cnst_b 0
ret2
.label gei2_ret_0
cnst_b 0
cnst_b 0
ret2


.code
.label GTI2
iaddrf_b 1 ; load A_high
iaddrf_b 3 ; load B_high
lt_w gti2_ret_0
iaddrf_b 0 ; load A_low
iaddrf_b 2 ; load B_low
le_w gti2_ret_0
cnst_b 1
cnst_b 0
ret2
.label gti2_ret_0
cnst_b 0
cnst_b 0
ret2

;;;;;;;;;;;;;;;;;;;;;;; COMMON 2 WORD COMPARISON ;;;;;;;;;;;;;;
.code
.label EQI2
.label EQU2
iaddrf_b 1 ; load A_high
iaddrf_b 3 ; load B_high
ne_w eqi2_ret_0
iaddrf_b 0 ; load A_low
iaddrf_b 2 ; load B_low
ne_w eqi2_ret_0
cnst_b 1
cnst_b 0
ret2
.label eqi2_ret_0
cnst_b 0
cnst_b 0
ret2


.code
.label NEI2
.label NEU2
iaddrf_b 1 ; load A_high
iaddrf_b 3 ; load B_high
eq_w nei2_ret_0
iaddrf_b 0 ; load A_low
iaddrf_b 2 ; load B_low
eq_w nei2_ret_0
cnst_b 1
cnst_b 0
ret2
.label nei2_ret_0
cnst_b 0
cnst_b 0
ret2

;;;;;;;;;;;;;;;; MULTIPLICATION - SLOW ;;;;;;;;;;;;;;;;;


;;;;; one word

.code
.label MULI1
.label MULU1
	
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


;;;;; two word



;;;;;;;;;;;;; DIVISION - SLOW ;;;;;;;;;;;;;;;;;;;;;;;

;;;;; one word

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

;;;;; two word


.code
.export ADDU2
.label ADDU2
.export ADDI2
.label ADDI2

iaddrf_b 0
iaddrf_b 2
add
dup
iaddrf_b 0
uge_w addu2_no_overflow

addrf_b 1
iaddrf_b 1
add_b 1
store

.label addu2_no_overflow 
iaddrf_b 1
iaddrf_b 3
add

ret2

.code
.export MULU2
.export MULI2
.label MULU2
.label MULI2

;;;; (AH * 2^16 + AL) * (BH * 2^16 + BL) = AH * BH * 2^32 + AH * BL * 2^16 + AL * BH * 2^16 + AL * BL
ret


