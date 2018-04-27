.data
.export __sp_before_int
.label __sp_before_int
.byte 1 0

.code
.export ei
.label ei
ei
ret

.export di
.label di
di
ret

.export __timer_interrupt_vector
.import timer_interrupt
.import sched_stack
.label __timer_interrupt_vector
; save interrupted sp
storesp_w sched_stack+1
; load scheduler sp

loadsp_w sched_stack
pushc ;will avoid writing to sched_stack+1 preserving prevSP, hopefully
ec
call0_w timer_interrupt
reti


; one at a time pls!
.export __system_interrupt_vector
.label  __system_interrupt_vector
.import system_interrupt_stack
; save interrupted sp
storesp_w system_interrupt_stack+1
; load scheduler sp
loadsp_w system_interrupt_stack

; access this shit via system_interrupt_stack[0] [1] ....

pushc
ec
call0_w system_interrupt
reti
