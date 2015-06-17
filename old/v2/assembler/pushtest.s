.text
add 0x4020 0 SP
add 0 0 cnt
L1:
push *cnt 0 0
add *cnt 1 cnt
jmp 0 0 L1
.data
cnt: .1
