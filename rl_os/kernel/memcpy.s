.label memcpy
.export memcpy
; ap : dst
; ap+1 : src
; ap+2 : n


iaddrf_b 1    ;load src ptr
dup
pops          ;store in S
iaddrf_b 2    ;load n
add           ;find end

iaddrf_b 0    ;load dst ptr
popd          ;store in D

.label _memcpy_loop
; end ptr on top of stack
dup               ; dup end ptr
pushs             ; push current S
eq_w _memcpy_end  ; reached end

sdmi              ; s->d move + inc

jump_w _memcpy_loop

.label _memcpy_end
ret
