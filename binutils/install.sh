#!/bin/sh

mkdir -p /usr/local/rl_cpu/bin
cp ./asm ./trans ./asm_bc ./link /usr/local/rl_cpu/bin
mkdir -p /usr/local/rl_cpu/lib
cp ./rt.o ./progbeg.o /usr/local/rl_cpu/lib