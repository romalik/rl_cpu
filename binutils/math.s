.code
.export __builtin_MULI1
.export __builtin_MULU1
.label __builtin_MULI1
.label __builtin_MULU1
; A at bp-4 [addrl_b -4]
; B at bp-5 [addrl_b -5]
; res tmp bp [addrl_b 0]
; store retval at bp-5 [addrl_b -5]
cnst_b 0                       ;here we keep the result, accessable as local 1
.label mult_begin
iaddrl_b -4                     ;load A
cnst_b 0                       ;compare with 0
eq_w mult_end          ;A == 0 - jump to end

iaddrl_b -4                    ;load A again
band_b 1                       ;check LSB
cnst_b 0                       ;if equals to zero
eq_w skip                      ;jump to shift
                               ;else
iaddrl_b 0                     ;load current result
iaddrl_b -5                    ;load current B
add                                    ;sum them
addrl_b 0                      ;store new result
rstore                         ;

.label skip

iaddrl_b -5                     ;load B again
lsh                             ;shift it left
addrl_b -5                      ;store it back
rstore

iaddrl_b -4                     ;load A
rsh                             ;shift it right
addrl_b -4                      ;store A back
rstore

jump_w mult_begin

.label mult_end
iaddrl_b 0
addrl_b -5
rstore
ret


.code
.export __builtin_MULU2
.export __builtin_MULI2
.label __builtin_MULU2
.label __builtin_MULI2

ret



.export __builtin_LSHI1
.export __builtin_LSHU1
.label __builtin_LSHI1
.label __builtin_LSHU1
; n at bp-4 [addrl_b -4]
; res at bp-5 [addrl_b -5]
.label lshi1_loop
iaddrl_b -4     ;load n
cnst_b 0
eq_w lshi1_end  ;if zero, jump to end
iaddrl_b -5     ;load res
lsh             ;shift
addrl_b -5      ;and
rstore          ;store it
iaddrl_b -4     ;load n
sub_b 1         ;decrement
addrl_b -4      ;and
rstore          ;store
jump_w lshi1_loop
.label lshi1_end
ret




.export __builtin_LSHI2
.export __builtin_LSHU2
.label __builtin_LSHI2
.label __builtin_LSHU2

ret



.export __builtin_RSHI1
.export __builtin_RSHU1
.label __builtin_RSHI1
.label __builtin_RSHU1
; n at bp-4 [addrl_b -4]
; res at bp-5 [addrl_b -5]
.label rshi1_loop
iaddrl_b -4     ;load n
cnst_b 0
eq_w lshi1_end  ;if zero, jump to end
iaddrl_b -5     ;load res
rsh             ;shift
addrl_b -5      ;and
rstore          ;store it
iaddrl_b -4     ;load n
sub_b 1         ;decrement
addrl_b -4      ;and
rstore          ;store
jump_w rshi1_loop
.label rshi1_end
ret


.export __builtin_RSHI2
.export __builtin_RSHU2
.label __builtin_RSHI2
.label __builtin_RSHU2


ret



.export __builtin_DIVU1
.export __builtin_DIVI1
.label __builtin_DIVU1
.label __builtin_DIVI1

; A at bp-5 [addrl_b -5]
; B at bp-4 [addrl_b -4]
; res tmp bp,bp+1 [addrl_b 0][addrl_b 1]
; store retval at bp-5 [addrl_b -5]
cnst_b 0
cnst_b 0
jump_w $divu1_3
.label $divu1_2
addrl_b 1
iaddrl_b 1
add_b 1
store
addrl_b 0
iaddrl_b 0
iaddrl_b -4
add
store
.label $divu1_3
iaddrl_b 0
iaddrl_b -5
ule_w $divu1_2
addrl_b 1
iaddrl_b 1
sub_b 1
store
iaddrl_b 1
addrl_b -5
rstore
ret
.label $divu1_1

.export __builtin_MODU1
.export __builtin_MODI1
.label __builtin_MODU1
.label __builtin_MODI1
; A at bp-5 [addrl_b -5]
; B at bp-4 [addrl_b -4]
; res tmp bp,bp+1 [addrl_b 0][addrl_b 1]
; store retval at bp-5 [addrl_b -5]
iaddrl_b -5
dup  ;;iaddrl_b -5
iaddrl_b -4
call0_w __builtin_DIVU1
discard_b 1
iaddrl_b -4
call0_w __builtin_MULI1
discard_b 1
sub
addrl_b -5
rstore

ret


;;unsigned comparison

.export __builtin_cmp_GEU2
.label __builtin_cmp_GEU2
; Al bp-7 <----result here
; Ah bp-6
; Bl bp-5
; Bh bp-4
addrl_b -7  ;for result rstore
iaddrl_b -6 ;Ah
iaddrl_b -4 ;Bh
ult_w __builtin_cmp_false
iaddrl_b -6 ;Ah
iaddrl_b -4 ;Bh
ugt_w __builtin_cmp_true
iaddrl_b -7 ;Al
iaddrl_b -5 ;Bl
ult_w __builtin_cmp_false
jump_w __builtin_cmp_true


.export __builtin_cmp_GTU2
.label __builtin_cmp_GTU2
; Al bp-7 <----result here
; Ah bp-6
; Bl bp-5
; Bh bp-4
addrl_b -7  ;for result rstore
iaddrl_b -6 ;Ah
iaddrl_b -4 ;Bh
ult_w __builtin_cmp_false
iaddrl_b -6 ;Ah
iaddrl_b -4 ;Bh
ugt_w __builtin_cmp_true
iaddrl_b -7 ;Al
iaddrl_b -5 ;Bl
ule_w __builtin_cmp_false
jump_w __builtin_cmp_true


.export __builtin_cmp_LEU2
.label __builtin_cmp_LEU2
; Al bp-7 <----result here
; Ah bp-6
; Bl bp-5
; Bh bp-4
addrl_b -7  ;for result rstore
iaddrl_b -6 ;Ah
iaddrl_b -4 ;Bh
ugt_w __builtin_cmp_false
iaddrl_b -6 ;Ah
iaddrl_b -4 ;Bh
ult_w __builtin_cmp_true
iaddrl_b -7 ;Al
iaddrl_b -5 ;Bl
ugt_w __builtin_cmp_false
jump_w __builtin_cmp_true


.export __builtin_cmp_LTU2
.label __builtin_cmp_LTU2
; Al bp-7 <----result here
; Ah bp-6
; Bl bp-5
; Bh bp-4
addrl_b -7  ;for result rstore
iaddrl_b -6 ;Ah
iaddrl_b -4 ;Bh
ugt_w __builtin_cmp_false
iaddrl_b -6 ;Ah
iaddrl_b -4 ;Bh
ult_w __builtin_cmp_true
iaddrl_b -7 ;Al
iaddrl_b -5 ;Bl
uge_w __builtin_cmp_false
jump_w __builtin_cmp_true


;;signed comparison

.export __builtin_cmp_GEI2
.label __builtin_cmp_GEI2
; Al bp-7 <----result here
; Ah bp-6
; Bl bp-5
; Bh bp-4
addrl_b -7  ;for result rstore
iaddrl_b -6 ;Ah
iaddrl_b -4 ;Bh
lt_w __builtin_cmp_false
iaddrl_b -6 ;Ah
iaddrl_b -4 ;Bh
gt_w __builtin_cmp_true
iaddrl_b -7 ;Al
iaddrl_b -5 ;Bl
eq_w __builtin_cmp_true
iaddrl_b -7 ;Al
iaddrl_b -5 ;Bl
ule_w __builtin_cmp_gei_cnt
; al > bl
iaddrl_b -6  ;check sign
band_w 0x8000
cnst_b 0
ne_w __builtin_cmp_false    ;negative
jump_w __builtin_cmp_true   ;positive

.label __builtin_cmp_gei_cnt
; al < bl
iaddrl_b -6  ;check sign
band_w 0x8000
cnst_b 0
ne_w __builtin_cmp_true     ;negative
jump_w __builtin_cmp_false  ;positive


.export __builtin_cmp_GTI2
.label __builtin_cmp_GTI2
; Al bp-7 <----result here
; Ah bp-6
; Bl bp-5
; Bh bp-4
addrl_b -7  ;for result rstore
iaddrl_b -6 ;Ah
iaddrl_b -4 ;Bh
lt_w __builtin_cmp_false
iaddrl_b -6 ;Ah
iaddrl_b -4 ;Bh
gt_w __builtin_cmp_true
iaddrl_b -7 ;Al
iaddrl_b -5 ;Bl
eq_w __builtin_cmp_false
iaddrl_b -7 ;Al
iaddrl_b -5 ;Bl
ule_w __builtin_cmp_gti_cnt
; al > bl
iaddrl_b -6  ;check sign
band_w 0x8000
cnst_b 0
ne_w __builtin_cmp_false    ;negative
jump_w __builtin_cmp_true   ;positive

.label __builtin_cmp_gti_cnt
; al < bl
iaddrl_b -6  ;check sign
band_w 0x8000
cnst_b 0
ne_w __builtin_cmp_true     ;negative
jump_w __builtin_cmp_false  ;positive



.export __builtin_cmp_LEI2
.label __builtin_cmp_LEI2
; Al bp-7 <----result here
; Ah bp-6
; Bl bp-5
; Bh bp-4
addrl_b -7  ;for result rstore
iaddrl_b -6 ;Ah
iaddrl_b -4 ;Bh
gt_w __builtin_cmp_false
iaddrl_b -6 ;Ah
iaddrl_b -4 ;Bh
lt_w __builtin_cmp_true
iaddrl_b -7 ;Al
iaddrl_b -5 ;Bl
eq_w __builtin_cmp_true
iaddrl_b -7 ;Al
iaddrl_b -5 ;Bl
uge_w __builtin_cmp_lei_cnt
; al < bl
iaddrl_b -6  ;check sign
band_w 0x8000
cnst_b 0
ne_w __builtin_cmp_false    ;negative
jump_w __builtin_cmp_true   ;positive

.label __builtin_cmp_lei_cnt
; al > bl
iaddrl_b -6  ;check sign
band_w 0x8000
cnst_b 0
ne_w __builtin_cmp_true     ;negative
jump_w __builtin_cmp_false  ;positive


.export __builtin_cmp_LTI2
.label __builtin_cmp_LTI2
; Al bp-7 <----result here
; Ah bp-6
; Bl bp-5
; Bh bp-4
addrl_b -7  ;for result rstore
iaddrl_b -6 ;Ah
iaddrl_b -4 ;Bh
gt_w __builtin_cmp_false
iaddrl_b -6 ;Ah
iaddrl_b -4 ;Bh
lt_w __builtin_cmp_true
iaddrl_b -7 ;Al
iaddrl_b -5 ;Bl
eq_w __builtin_cmp_false
iaddrl_b -7 ;Al
iaddrl_b -5 ;Bl
uge_w __builtin_cmp_lti_cnt
; al < bl
iaddrl_b -6  ;check sign
band_w 0x8000
cnst_b 0
ne_w __builtin_cmp_false    ;negative
jump_w __builtin_cmp_true   ;positive

.label __builtin_cmp_lti_cnt
; al > bl
iaddrl_b -6  ;check sign
band_w 0x8000
cnst_b 0
ne_w __builtin_cmp_true     ;negative
jump_w __builtin_cmp_false  ;positive




;;universal comparisons

.export __builtin_cmp_EQU2
.export __builtin_cmp_EQI2
.label __builtin_cmp_EQU2
.label __builtin_cmp_EQI2
; Al bp-7 <----result here
; Ah bp-6
; Bl bp-5
; Bh bp-4
addrl_b -7  ;for result rstore
iaddrl_b -6 ;Ah
iaddrl_b -4 ;Bh
ne_w __builtin_cmp_false
iaddrl_b -7 ;Al
iaddrl_b -5 ;Bl
ne_w __builtin_cmp_false
jump_w __builtin_cmp_true

.export __builtin_cmp_NEU2
.export __builtin_cmp_NEI2
.label __builtin_cmp_NEU2
.label __builtin_cmp_NEI2
; Al bp-7 <----result here
; Ah bp-6
; Bl bp-5
; Bh bp-4
addrl_b -7  ;for result rstore
iaddrl_b -6 ;Ah
iaddrl_b -4 ;Bh
ne_w __builtin_cmp_true
iaddrl_b -7 ;Al
iaddrl_b -5 ;Bl
ne_w __builtin_cmp_true
jump_w __builtin_cmp_false





.label __builtin_cmp_true
cnst_b 1
store
ret
.label __builtin_cmp_false
cnst_b 0
store
ret



