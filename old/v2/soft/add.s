#ifndef __ADD_S
#define __ADD_S
#include "defs.inc"


.text
add:
FUNC_HEADER()
push 0 0 0
add *LOCAL(1) 1 LOCAL(1)
add *ARG(1) *ARG(2) ARG(3)
FUNC_FOOTER()
#endif
