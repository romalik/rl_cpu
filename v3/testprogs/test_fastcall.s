.code
.export main
.label main
alloc_b 1
cnst_b 97
call_w addFive
addrl_b 0
rstore
call_w putc
.label loop
jump_w loop
.label addFive
iaddrl_w -4
add_b 5
ret
.label mult

