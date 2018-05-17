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


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;; TIMER INTERRUPT ;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
.export __timer_interrupt_vector
.import timer_interrupt
.import sched_stack
.label __timer_interrupt_vector
di
; save interrupted sp
storesp_w sched_stack+1
; load scheduler sp

loadsp_w sched_stack
pushc ;will avoid writing to sched_stack+1 preserving prevSP, hopefully
ec
call0_w timer_interrupt
reti
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;; MMU INTERRUPT ;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
.export __mmu_interrupt_vector
.import mmu_interrupt
.import mmu_stack
.label __mmu_interrupt_vector
di
; save interrupted sp
storesp_w mmu_stack+1
; load scheduler sp

loadsp_w mmu_stack
pushc
pushmmu
ec
call0_w mmu_interrupt
discard_b 1
reti
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;; DECODER INTERRUPT ;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
.export __decoder_interrupt_vector
.import decoder_interrupt
.import decoder_stack
.label __decoder_interrupt_vector
di
; save interrupted sp
storesp_w decoder_stack+1
; load scheduler sp

loadsp_w decoder_stack
pushc
ec
call0_w decoder_interrupt
reti
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;




;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;; UART  INTERRUPT ;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

.export __uart_interrupt_vector
.import uart_interrupt
.import uart_interrupt_stack
.label __uart_interrupt_vector
; save interrupted sp
di
storesp_w uart_interrupt_stack+1

loadsp_w uart_interrupt_stack
pushc ;will avoid writing to uart_interrupt_stack+1 preserving prevSP, hopefully
ec
call0_w uart_interrupt
reti
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;; UART2 INTERRUPT ;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

.export __uart2_interrupt_vector
.import uart2_interrupt
.import uart_interrupt_stack
.label __uart2_interrupt_vector
; save interrupted sp
di
storesp_w uart_interrupt_stack+1

loadsp_w uart_interrupt_stack
pushc ;will avoid writing to uart_interrupt_stack+1 preserving prevSP, hopefully
ec
call0_w uart2_interrupt
reti
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;




.import in_sys_int
.import out_sys_int
; one at a time pls!
.export __system_interrupt_vector
.label  __system_interrupt_vector
.import system_interrupt_stack
di
; save interrupted sp
storesp_w system_interrupt_stack+1
; load scheduler sp
loadsp_w system_interrupt_stack

; access this shit via system_interrupt_stack[0] [1] ....

pushc
ec
; call0_w in_sys_int
call0_w system_interrupt
; call0_w out_sys_int
reti
