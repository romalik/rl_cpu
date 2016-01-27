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
;di
call0_w __timer_interrupt_wrapper
;ei

.import __builtin_memcpy
.label __timer_interrupt_wrapper
cnst_w sched_stack
addrl_b -7
cnst_b 4
call0_w __builtin_memcpy

loadsp_w sched_stack+4

;; this shit would work great, if my compiler supported pass-by-reference (func(int &a) {..}). But it does not. So, slow, but cross-compiler version here
cnst_w sched_stack+4
popbp

addrl_b -4 ;SP PC BP AP
call0_w timer_interrupt
discard_b 1

reti



.export __system_interrupt_vector
.label  __system_interrupt_vector
;di
call0_w system_interrupt
;ei
reti


.export __system_interrupt_wrapper
.import system_interrupt
.label  __system_interrupt_wrapper

; Stack now: [SP,PC,BP,AP int] [PC ret] [AP ret] [BP ret] <addrl 0>

iaddrf_b 0 ;pointer to syscall struct
addrl_b -7 ;SP PC BP AP
call0_w system_interrupt

; Stack now: [SP,PC,BP,AP int] [PC ret] [AP ret] [BP ret] [addrl 0] [addrl 1] <sp>

discard_b 5 ;  PC AP BP SyscallStructPtr IntFramePtr

reti

