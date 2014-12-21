#include "defs.inc"

.text
_start:
add 5 0 Var1
shl(*Var1, Var2)
shr(*Var1, Var3)
shl8(*Var1, Var4)
shr8(*Var1, Var5)

L: jmp 0 0 L

.data
Var1: .1
Var2: .1
Var3: .1
Var4: .1
Var5: .1


