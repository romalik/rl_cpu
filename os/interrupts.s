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
call0_w __timer_interrupt_wrapper

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
        addrl_b -1 ;AP
        addrl_b -2 ;BP
        addrl_b -3 ;PC
        addrl_b -4 ;SP
call0_w timer_interrupt
        discard_b 4
reti



.export __system_interrupt_vector
.import system_interrupt
.label  __system_interrupt_vector
call0_w system_interrupt
reti


