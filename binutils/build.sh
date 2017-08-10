#!/bin/sh

g++ -o asm ./assembler.cpp
#g++ -o trans ./translator.cpp
g++ -std=c++11 -o link ./linker.cpp
g++ -o trans ./trans2.cpp
./asm kernbeg.s kernbeg.o
./asm progbeg.s progbeg.o
./asm kernend.s kernend.o
./asm progend.s progend.o
./asm rt.s rt.o
./asm math.s ./math.o
