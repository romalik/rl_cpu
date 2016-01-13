#!/bin/sh

mkdir -p /usr/local/rl_cpu/bin
cp ./asm ./trans ./asm_bc ./link /usr/local/rl_cpu/bin
mkdir -p /usr/local/rl_cpu/lib
cp ./math.o ./rt.o ./kernbeg.o ./progbeg.o ./progend.o /usr/local/rl_cpu/lib
