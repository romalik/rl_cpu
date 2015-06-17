.export putc
.code 
.proc putc 0 0
.label putc
cnst_w 65535
cnst_w 0
discard1
iaddrf_b 0
store
cnst_b 0
ret
.label $1
.endproc putc 0 0
.export puts
.proc puts 0 1
.label puts
alloc_b 1
addrf_b 0
iaddrf_b 0
store
jump_w $4
.label $3
iaddrf_b 0
indir
addrl_b 0
rstore
call_w putc
discard_b 1
addrf_b 0
iaddrf_b 0
add_b 1
store
.label $4
iaddrf_b 0
indir
cnst_b 0
ne_w $3
cnst_b 0
ret
.label $2
.endproc puts 0 1
.data 
.export myString
.align 1
.label myString
.byte 1 72
.byte 1 101
.byte 1 108
.byte 1 108
.byte 1 111
.byte 1 32
.byte 1 102
.byte 1 114
.byte 1 111
.byte 1 109
.byte 1 32
.byte 1 114
.byte 1 108
.byte 1 67
.byte 1 80
.byte 1 85
.byte 1 33
.byte 1 0
.export main
.code 
.proc main 0 1
.label main
alloc_b 1
cnst_w myString
addrl_b 0
rstore
call_w puts
discard_b 1
.label $7
.label $8
jump_w $7
cnst_b 0
ret
.label $6
.endproc main 0 1
