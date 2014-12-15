.text

add 0 0 Counter
L1:
add *Counter 1 Counter
jl *Counter 0x10 L1
add 5 0 Counter
jmp 0 0 L1

.data
Counter: .1
