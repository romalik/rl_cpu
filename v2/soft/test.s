#include "defs.inc"
#include "init.s"
.text
_start:
push 0x15 0 0
call func 0 0
add 0x20 0 Var3
L1: jmp 0 0 L1

func:
FUNC_HEADER()
push 0x10 0 0 # local
add *LOCAL(1) 1 LOCAL(1)
add *LOCAL(1) 0 Var1
add *ARG(1) *LOCAL(1) Var2
FUNC_FOOTER()

.data
Var1: .1
Var2: .1
Var3: .1
