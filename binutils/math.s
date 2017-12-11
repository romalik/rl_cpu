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


;// long a = 0x12345678
;// *(int *)&a == 0x5678
;// *((int *)&a+1) == 0x1234
;unsigned long shl2(unsigned long a, unsigned int n) {
;  unsigned int * rl = (unsigned int *)&a;
;  unsigned int * rh = ((unsigned int *)&a)+1;
;
;  unsigned int carry;
;  while(n) {
;        carry = 0;
;        if((*rl)&0x8000) carry = 1;
;
;        *rl = (*rl) << 1;
;        *rh = ((*rh) << 1) + carry;
;        n--;
;  }
;  return a;
;}
;
;unsigned long shr2(unsigned long a, unsigned int n) {
;  unsigned int * rl = (unsigned int *)&a;
;  unsigned int * rh = ((unsigned int *)&a)+1;
;
;  unsigned int carry;
;  while(n) {
;        carry = 0;
;        if((*rh)&0x0001) carry = 0x8000;
;
;        *rl = ((*rl) >> 1) + carry;
;        *rh = ((*rh) >> 1);
;        n--;
;  }
;  return a;
;}
;
;
;
;unsigned long mul2(unsigned long a, unsigned long b) {
;  unsigned long res = 0;
;  while(b) {
;    if(b&0x0001) {
;      res += a;
;    }
;    a = shl2(a,1);
;    b = shr2(b,1);
;  }
;  return res;
;}





.code
.export __builtin_MULU2
.export __builtin_MULI2
.label __builtin_MULU2
.label __builtin_MULI2

; Al bp-7 <----result here
; Ah bp-6
; Bl bp-5
; Bh bp-4
alloc_b 3
addrl_b 0
cnst_b 0
cnst_b 0
store2 
jump_w $18
.label $17
addrl_b -5
indir2 
cnst_b 1
cnst_b 0
band2 
cnst_b 0
cnst_b 0
call0_w __builtin_cmp_EQU2 $20
discard_b 3
cnst_b 0
ne_w $20
addrl_b 0
addrl_b 0
indir2 
addrl_b -7
indir2 
add2 
store2 
.label $20
addrl_b 2
cnst_b 1
store 
addrl_b -7
addrl_b -7
indir2 
iaddrl_b 2
call0_w __builtin_LSHU2
discard_b 1
store2 
addrl_b -5
addrl_b -5
indir2 
iaddrl_b 2
call0_w __builtin_RSHU2
discard_b 1
store2 
.label $18
addrl_b -5
indir2 
cnst_b 0
cnst_b 0
call0_w __builtin_cmp_NEU2 $17
discard_b 3
cnst_b 0
ne_w $17
addrl_b 0
indir2 
addrl_b -7
rstore2 
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
; Al bp-6 <- ret
; Ah bp-5
; n  bp-4

alloc_b 5
addrl_b 0
addrl_b -6
store 
addrl_b 2
addrl_b -5
store 
jump_w $5
.label $4
addrl_b 1
cnst_b 0
store 
iaddrl_b 0
indir 
cnst_b 0
cnst_w 32768
cnst_b 0
band2 
cnst_b 0
cnst_b 0
call0_w __builtin_cmp_EQI2 $7
discard_b 3
cnst_b 0
ne_w $7
addrl_b 1
cnst_b 1
store 
.label $7
iaddrl_b 0
iaddrl_b 0
indir 
lsh 
store 
iaddrl_b 2
iaddrl_b 2
indir 
lsh 
iaddrl_b 1
add 
store 
addrl_b -4
iaddrl_b -4
sub_b 1
store 
.label $5
iaddrl_b -4
cnst_b 0
ne_w $4
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

; Al bp-6 <- ret
; Ah bp-5
; n  bp-4
alloc_b 6
addrl_b 2
addrl_b -6
store 
addrl_b 0
addrl_b -5
store 
jump_w $12
.label $11
addrl_b 3
cnst_b 0
store 
addrl_b 1
iaddrl_b 3
store 
iaddrl_b 0
indir 
band_b 1
iaddrl_b 3
eq_w $14
addrl_b 1
cnst_w 32768
store 
.label $14
iaddrl_b 2
iaddrl_b 2
indir 
rsh 
iaddrl_b 1
add 
store 
iaddrl_b 0
iaddrl_b 0
indir 
rsh 
store 
addrl_b -4
iaddrl_b -4
sub_b 1
store 
.label $12
iaddrl_b -4
cnst_b 0
ne_w $11
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



;unsigned long divide(unsigned long dividend, unsigned long divisor) {
;
;    unsigned long denom=divisor;
;    unsigned long current = 1;
;    unsigned long answer=0;
;
;    if ( denom > dividend)
;        return 0;
;
;    if ( denom == dividend)
;        return 1;
;
;    while (denom <= dividend) {
;        denom <<= 1;
;        current <<= 1;
;    }
;
;    denom >>= 1;
;    current >>= 1;
;
;    while (current!=0) {
;        if ( dividend >= denom) {
;            dividend -= denom;
;            answer |= current;
;        }
;        current >>= 1;
;        denom >>= 1;
;    }
;    return answer;
;}
;
;unsigned long mod(unsigned long a, unsigned long b) {
;  return a - divide(a,b)*b; //awful
;}

.code
.export __builtin_DIVU2
.export __builtin_DIVI2
.label __builtin_DIVU2
.label __builtin_DIVI2
; Al bp-7 <----result here
; Ah bp-6
; Bl bp-5
; Bh bp-4

alloc_b 8
addrl_b 0
addrl_b -5
indir2 
store2
addrl_b 2
cnst_b 1
cnst_b 0
store2 
addrl_b 4
cnst_b 0
cnst_b 0
store2 
addrl_b 0
indir2 
addrl_b -7
indir2 
call0_w __builtin_cmp_LEU2 $2_d2
discard_b 3
cnst_b 0
ne_w $2_d2
cnst_b 0
cnst_b 0
addrl_b -7
rstore2 
ret 
.label $2_d2
addrl_b 0
indir2 
addrl_b -7
indir2 
call0_w __builtin_cmp_NEU2 $7_d2
discard_b 3
cnst_b 0
ne_w $7_d2
cnst_b 1
cnst_b 0
addrl_b -7
rstore2 
ret 
.label $6_d2
addrl_b 6
cnst_b 1
store 
addrl_b 0
addrl_b 0
indir2 
iaddrl_b 6
call0_w __builtin_LSHU2
discard_b 1
store2 
addrl_b 2
addrl_b 2
indir2 
iaddrl_b 6
call0_w __builtin_LSHU2
discard_b 1
store2 
.label $7_d2
addrl_b 0
indir2 
addrl_b -7
indir2 
call0_w __builtin_cmp_LEU2 $6_d2
discard_b 3
cnst_b 0
ne_w $6_d2
addrl_b 6
cnst_b 1
store 
addrl_b 0
addrl_b 0
indir2 
iaddrl_b 6
call0_w __builtin_RSHU2
discard_b 1
store2 
addrl_b 2
addrl_b 2
indir2 
iaddrl_b 6
call0_w __builtin_RSHU2
discard_b 1
store2 
jump_w $10_d2
.label $9_d2
addrl_b -7
indir2 
addrl_b 0
indir2 
call0_w __builtin_cmp_LTU2 $12_d2
discard_b 3
cnst_b 0
ne_w $12_d2
addrl_b -7
addrl_b -7
indir2 
addrl_b 0
indir2 
sub2 
store2 
addrl_b 4
addrl_b 4
indir2 
addrl_b 2
indir2 
bor2 
store2 
.label $12_d2
addrl_b 7
cnst_b 1
store 
addrl_b 2
addrl_b 2
indir2 
iaddrl_b 7
call0_w __builtin_RSHU2
discard_b 1
store2 
addrl_b 0
addrl_b 0
indir2 
iaddrl_b 7
call0_w __builtin_RSHU2
discard_b 1
store2 
.label $10_d2
addrl_b 2
indir2 
cnst_b 0
cnst_b 0
call0_w __builtin_cmp_NEU2 $9_d2
discard_b 3
cnst_b 0
ne_w $9_d2
addrl_b 4
indir2 
addrl_b -7
rstore2 
ret 

.code
.export __builtin_MODU2
.export __builtin_MODI2
.label __builtin_MODU2
.label __builtin_MODI2
alloc_b 4
addrl_b 0
addrl_b -7
indir2 
store2 
addrl_b 2
addrl_b -5
indir2 
store2 
addrl_b 0
indir2 
addrl_b 0
indir2 
addrl_b 2
indir2 
call0_w __builtin_DIVU2
discard_b 2
addrl_b 2
indir2 
call0_w __builtin_MULU2
discard_b 2
sub2 
addrl_b -7
rstore2 
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

.export __builtin_cmp_EQF2
.export __builtin_cmp_LTF2
.export __builtin_cmp_NEF2
.export __builtin_cmp_LEF2
.export __builtin_DIVF2
.export __builtin_MULF2
.export __builtin_cmp_GEF2
.export __builtin_cmp_GTF2

.label __builtin_cmp_EQF2
.label __builtin_cmp_LTF2
.label __builtin_cmp_NEF2
.label __builtin_cmp_LEF2
.label __builtin_DIVF2
.label __builtin_MULF2
.label __builtin_cmp_GEF2
.label __builtin_cmp_GTF2
ret

