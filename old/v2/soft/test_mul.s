#include "defs.inc"
#include "init.s"
#include "mul.s"
#include "add.s"
.text
_start:
push 0 0 0
push 3 0 0
push 4 0 0
call mul 0 0
add SP 2 SP
pop Var1 0 0


push 0 0 0
push 2 0 0
push 3 0 0
call mul 0 0
add SP 2 SP
pop Var2 0 0

L: jmp 0 0 L
.data
Var1: .1
Var2: .1
