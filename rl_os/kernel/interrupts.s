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
.import resched
.import sched_stack
.label __timer_interrupt_vector
di
call0_w __timer_interrupt_wrapper
ei
reti


.import __builtin_memcpy
.import ticks
.import ticksToSwitch
.label __timer_interrupt_wrapper

; ticks++
; if ticksToSwitch
; ticksToSwitch--
; if(ticksToSwitch == 0) 
;    call0_w resched;

cnst_w ticks
icnst_w ticks
add_b 1
store

icnst_w ticksToSwitch
cnst_b 0
eq_w __tiwrapper_exit

cnst_w ticksToSwitch
icnst_w ticksToSwitch
sub_b 1
store

icnst_w ticksToSwitch
cnst_b 0
gt_w __tiwrapper_exit


cnst_w sched_stack
addrl_b -7
cnst_b 4
call0_w __builtin_memcpy

loadsp_w sched_stack+4

;; this shit would work great, if my compiler supported pass-by-reference (func(int &a) {..}). But it does not. So, slow, but cross-compiler version here
cnst_w sched_stack+4
popbp

addrl_b -4 ;SP PC BP AP
call0_w resched 
discard_b 1
ei
reti

.label __tiwrapper_exit
ret


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

