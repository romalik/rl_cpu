#!/bin/sh
lcc -I../stdlib/ ../stdlib/stdio.o ../stdlib/string.o ./kernel_main.c ./heap.c ./ata.c ./sh.c ./rlfs.c -o image
