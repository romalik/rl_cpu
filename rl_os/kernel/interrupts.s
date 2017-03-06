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
addrl_b -9
cnst_b 6
call0_w __builtin_memcpy



loadsp_w sched_stack+6

cnst_w sched_stack+6
popbp

cnst_w sched_stack
call0_w resched 
discard_b 1
ei
reti

.label __tiwrapper_exit
ret


.export __system_interrupt_vector
.label  __system_interrupt_vector
;di
iaddrf_b 0 ;pointer to syscall struct
addrs_b -7 ; ->SP PC BP AP S D [ptr to scall]
call0_w system_interrupt
;ei
discard_b 2
reti



