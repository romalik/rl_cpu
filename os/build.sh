#!/bin/sh


../format.sh -i *.c
../format.sh -i *.h

asm interrupts.s interrupts.o
asm syscall.s syscall.o


lcc -Wo"-kernel" -I./ ./rlfs3.c ./blk.c  ./mm.c ./kernel_worker.c ./fork.c ./syscall.o ./exit.c ./kstdio.c ./sched.c ./sys.c ./string.c ./interrupts.o ./kernel_main.c ./malloc.c ./ata.c ./sh.c -o image

lcc -I./ ./fork.c ./syscall.o ./exit.c ./stdio.c ./string.c ./malloc.c ./hello.c -o hello.bin  -Wl"-text" -Wl"0x8000" -Wl"-data" -Wl"0x8000"
lcc -I./ ./fork.c ./syscall.o ./exit.c ./stdio.c ./string.c ./malloc.c ./task1.c -o task1.bin  -Wl"-text" -Wl"0x8000" -Wl"-data" -Wl"0x8000"

./genhex.sh ./hello.bin > ./hello.hex
./genhex.sh ./task1.bin > ./task1.hex
