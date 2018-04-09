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
storesp_w __sp_before_int
; load scheduler sp
loadsp_w sched_stack

; access this shit via sched_stack[0] [1] ....
cnst_b 0 ;;mmuSelector
icnst_w __sp_before_int ;;sp
call0_w timer_interrupt
reti


; one at a time pls!
.export __system_interrupt_vector
.label  __system_interrupt_vector
.import system_interrupt_stack
; save interrupted sp
storesp_w __sp_before_int
; load scheduler sp
loadsp_w system_interrupt_stack

; access this shit via system_interrupt_stack[0] [1] ....
cnst_b 0 ;;mmuSelector
icnst_w __sp_before_int ;;sp
pushap ;;scallPtr
call0_w system_interrupt
discard_b 1
reti
