#!/bin/sh

g++ -O2 -std=c++11 -o asm ./assembler.cpp
#g++ -o trans ./translator.cpp
g++ -O2 -std=c++11 -o link ./linker.cpp
g++ -O2 -o trans ./trans2.cpp
./asm kernbeg.s kernbeg.o
./asm progbeg.s progbeg.o
./asm kernend.s kernend.o
./asm progend.s progend.o
./asm rt.s rt.o
./asm math.s ./math.o
