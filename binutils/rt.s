.code
.export halt
.label halt
jump_w halt


.label __builtin_memcpy
.export __builtin_memcpy
; bp-4 : n [addrl_b -4]
; bp-5 : src [addrl_b -5]
; bp-6 : dest [addrl_b -6]

iaddrl_b -5   ;load src ptr
dup           
pops          ;store in S
iaddrl_b -4   ;load n
add           ;find end

iaddrl_b -6   ;load dst ptr
popd          ;store in D

.label _builtin_memcpy_loop
; end ptr on top of stack
dup               ; dup end ptr
pushs             ; push current S
eq_w _builtin_memcpy_end  ; reached end

sdmi              ; s->d move + inc

jump_w _builtin_memcpy_loop

.label _builtin_memcpy_end
ret
