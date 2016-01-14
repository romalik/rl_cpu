.code
.export syscall
.label syscall
iaddrf_b 0 ;load addr of syscall struct
syscall
ret

.export runBin
.label runBin
jump_w 0x8000

