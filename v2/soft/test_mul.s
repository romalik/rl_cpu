#include "defs.inc"
#include "mul.s"
.text
push 0 0 0
push 3 0 0
push 4 0 0
call mul 0 0
pop Var1 0 0
L: jmp 0 0 L
.data
Var1: .1
