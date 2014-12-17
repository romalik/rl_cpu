#ifndef __MUL_S
#define __MUL_S
#include "defs.inc"

#C = A * B
#ARG -1 - A
#ARG -2 - B
#ARG -3 - C
#LOCAL +1 - tmp

.text
mul:
FUNC_HEADER()
push 0 0 0 #D
add 0 0 ARG(3) #clear C
mul_check:
je *ARG(2) 0 mul_end # B == 0? -> end
and *ARG(2) 1 LOCAL(1)
je *LOCAL(1) 0 mul_shift
add *ARG(1) *ARG(2) ARG(3)
mul_shift:
shl(*ARG(1),ARG(1))
shr(*ARG(2),ARG(2))
jmp 0 0 mul_check
mul_end:
FUNC_FOOTER()

