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

add **BP+2 0 bp2
add **BP+3 0 bp3
add **BP+4 0 bp4
L1: jmp 0 0 L1
#epilog
sub *BP 1 SP # free locals
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
sub *BP 1 SP
pop BP 0 0
ret 0 0 0
.data
bp2: .1
bp3: .1
bp4: .1
Result: .1
