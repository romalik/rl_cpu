.text
add 0x4100 0 SP
call add_caller 0 0
loop: jmp 0 0 loop
add:
#prolog
push *BP 0 0
add SP 1 BP
push 123 0 0 # local
push 32 0 0 # local
push 111 0 0 # local


add **BP+2 **BP+3 *BP+4
L3: jmp 0 0 L3

#epilog
add **BP 0 SP # free locals
pop BP 0 0
ret 0 0 0


add_caller:
push *BP 0 0
add SP 1 BP

push 0 0 0 # result [BP - 1]
push 1 0 0 # param1
push 2 0 0 # param2
call add 0 0
add SP 2 SP
add **BP-1 0 Result
add SP 1 SP
pop BP 0 0
ret 0 0 0
.data
Result: .0
