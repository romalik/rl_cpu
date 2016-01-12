#!/bin/sh

asm interrupts.s interrupts.o
lcc -I./ ./stdio.c ./syscall.c ./string.c ./interrupts.o ./kernel_main.c ./malloc.c ./ata.c ./sh.c ./rlfs.c -o image
