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
.label __timer_interrupt_vector
call0_w timer_interrupt
jump

.export __system_interrupt_vector
.import system_interrupt
.label  __system_interrupt_vector
call0_w system_interrupt
jump


