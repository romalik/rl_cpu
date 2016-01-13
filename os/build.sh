#!/bin/sh

asm interrupts.s interrupts.o
asm syscall.s syscall.o

lcc -I./ ./syscall.o ./stdio.c ./sys.c ./string.c ./interrupts.o ./kernel_main.c ./malloc.c ./ata.c ./sh.c ./rlfs.c -o image

lcc -I./ ./syscall.o ./stdio.c ./string.c ./malloc.c ./hello.c -o hello.bin -text 0x8000

./genhex.sh ./hello.bin > ./hello.hex
