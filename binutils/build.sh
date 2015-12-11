#!/bin/sh

g++ -o asm ./assembler.cpp
g++ -o trans ./translator.cpp
g++ -o link ./linker.cpp
g++ -o trans ./trans2.cpp
./asm progbeg.s progbeg.o
./asm rt.s rt.o
./asm math.s ./math.o
