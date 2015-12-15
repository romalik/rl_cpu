.code
.export halt
.label halt
jump_w halt

.label __builtin_memcpy
.export __builtin_memcpy
; bp-4 : n [addrl_b -4]
; bp-5 : src [addrl_b -5]
; bp-6 : dest [addrl_b -6]

cnst_b 0      ;store curr word idx as local 1
.label _memcpy_loop
iaddrl_b 0    ;load curr word idx
iaddrl_b -4   ;load n
eq_w _memcpy_end

iaddrl_b -5 ;load src addr
iaddrl_b 0  ;load current word idx
add         ;sum
indir

iaddrl_b -6 ;load dest addr
iaddrl_b 0  ;load current word idx
add         ;sum

rstore      

iaddrl_b 0  ;increase curr word idx
add_b 1
addrl_b 0
rstore      ;store it

jump_w _memcpy_loop

.label _memcpy_end
ret
