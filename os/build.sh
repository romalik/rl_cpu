#!/bin/sh


../format.sh -i *.c
../format.sh -i *.h

asm interrupts.s interrupts.o
asm syscall.s syscall.o


lcc -Wo"-kernel" -I./ ./tty.c ./rlfs3.c ./blk.c  ./mm.c ./kernel_worker.c ./fork.c ./syscall.o ./exit.c ./kstdio.c ./sched.c ./sys.c ./string.c ./interrupts.o ./kernel_main.c ./malloc.c ./ata.c ./sh_builtin.c -o image

lcc -I./ ./fork.c ./syscall.o ./exit.c ./stdio.c ./string.c ./malloc.c ./hello.c -o hello.bin  -Wl"-text" -Wl"0x8000" -Wl"-data" -Wl"0x8000"
lcc -I./ ./exec.c ./fork.c ./syscall.o ./exit.c ./stdio.c ./string.c ./malloc.c ./task1.c -o task1.bin  -Wl"-text" -Wl"0x8000" -Wl"-data" -Wl"0x8000"

lcc -I./ ./waitpid.c ./exec.c ./fork.c ./syscall.o ./exit.c ./stdio.c ./string.c ./malloc.c ./sh.c -o sh.bin  -Wl"-text" -Wl"0x8000" -Wl"-data" -Wl"0x8000"

# ./genhex.sh ./hello.bin > ./hello.hex
# ./genhex.sh ./task1.bin > ./task1.hex

./genhex.sh ./sh.bin > ./sh.hex

cp ./hello.bin ./task1.bin ./sh.bin ./root


g++ -o make_rootfs ./make_rootfs.cpp && ./make_rootfs hdd 10000000 root 

