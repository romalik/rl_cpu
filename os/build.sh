#!/bin/sh

asm interrupts.s interrupts.o
lcc -I../stdlib/ ../stdlib/stdio.o ../stdlib/string.o ./interrupts.o ./kernel_main.c ./malloc.c ./ata.c ./sh.c ./rlfs.c -o image
