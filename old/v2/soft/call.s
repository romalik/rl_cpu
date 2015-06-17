.text
add 0x4100 0 SP
push 0x201 0 0
push 0x202 0 0
push 0x203 0 0
push 0x204 0 0
call func 0 0
loop: jmp 0 0 loop
func:
#prolog
push *BP 0 0
add SP 1 BP
push 0x101 0 0 # local
push 0x102 0 0 # local
push 0x103 0 0 # local

add **BP-3 0 bp_3
add **BP-2 0 bp_2
add **BP-1 0 bp_1
add **BP 0 bp0
add **BP+1 0 bp1
add **BP+2 0 bp2
add **BP+3 0 bp3
add **BP+4 0 bp4
#epilog
sub *BP 1 SP # free locals
pop BP 0 0
ret 0 0 0

.data
bp_3: .1
bp_2: .1
bp_1: .1
bp0: .1
bp1: .1
bp2: .1
bp3: .1
bp4: .1
