.code
.export syscall
.label syscall
iaddrf_b 0
syscall
ret

.export runBin
.label runBin
jump_w 0x8000

