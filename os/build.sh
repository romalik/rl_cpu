#!/bin/sh


../format.sh -i *.c
../format.sh -i *.h

asm interrupts.s interrupts.o
asm syscall.s syscall.o

lcc -Wo"-kernel" -I./ ./tty.c ./rlfs3.c ./blk.c  ./mm.c ./kernel_worker.c ./syscall.o ./kstdio.c ./sched.c ./sys.c ./string.c ./interrupts.o ./kernel_main.c ./malloc.c ./ata.c ./sh_builtin.c -o image

lcc -I./ ./syscall.o ./unistd.c ./stdio.c ./string.c ./malloc.c ./hello.c -o hello  -Wl"-text" -Wl"0x8000" -Wl"-data" -Wl"0x8000"
lcc -I./ ./syscall.o ./unistd.c ./stdio.c ./string.c ./malloc.c ./task1.c -o task1  -Wl"-text" -Wl"0x8000" -Wl"-data" -Wl"0x8000"

lcc -I./ ./syscall.o ./unistd.c ./stdio.c ./string.c ./malloc.c ./sh.c -o sh  -Wl"-text" -Wl"0x8000" -Wl"-data" -Wl"0x8000"
lcc -I./ ./syscall.o ./unistd.c ./stdio.c ./string.c ./malloc.c ./cat.c -o cat  -Wl"-text" -Wl"0x8000" -Wl"-data" -Wl"0x8000"
lcc -I./ ./syscall.o ./unistd.c ./stdio.c ./string.c ./malloc.c ./echo.c -o echo  -Wl"-text" -Wl"0x8000" -Wl"-data" -Wl"0x8000"

lcc -I./ ./syscall.o ./dirent.c ./unistd.c ./stdio.c ./string.c ./malloc.c ./ls.c -o ls  -Wl"-text" -Wl"0x8000" -Wl"-data" -Wl"0x8000"

cp ./cat ./hello ./task1 ./sh ./echo ./ls ./root

g++ -o make_rootfs ./make_rootfs.cpp && ./make_rootfs hdd 10000000 root 

